// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 *  Copyright 2013 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UPCEANExtension5Reader.h"

#include "UPCEANExtensionSupport.h"
#include <zxing/NotFoundException.h>
#include <zxing/ChecksumException.h>
#include <zxing/oned/OneDResultPoint.h>
#include <sstream>
#include <zxing/oned/UPCEANReader.h>

using namespace std;

namespace zxing {
	namespace oned {
		// Variables
		static const int EXTENSION_START_PATTERN_LENGTH = 3;
		static const int EXTENSION_START_PATTERN[EXTENSION_START_PATTERN_LENGTH] = {1,1,2};
		static const int CHECK_DIGIT_ENCODINGS[10] = {
			0x18, 0x14, 0x12, 0x11, 0x0C, 0x06, 0x03, 0x0A, 0x09, 0x05
		};
		
		// Methods
		UPCEANExtension5Reader::UPCEANExtension5Reader() : decodeMiddleCounters(4, 0) { }
		
		Ref<Result> UPCEANExtension5Reader::decodeRow(int rowNumber, Ref<BitArray> row, int rangeStart, int rangeEnd) {
			string& result = decodeRowStringBuffer;
			result.clear();
			int decodeOffset;
			try {
				decodeOffset = decodeMiddle(row, rangeStart, rangeEnd, result);
			} catch (ChecksumException exception) {
				throw NotFoundException();
			}
			
			ArrayRef< Ref<ResultPoint> > resultPoints(2);
			resultPoints[0] = Ref<OneDResultPoint>(new OneDResultPoint(rangeStart, rowNumber));
			resultPoints[1] =  Ref<OneDResultPoint>(new OneDResultPoint(decodeOffset, rowNumber));
			Ref<String> resultString(new String(result));
			
			// Compile result
			return Ref<Result>(new Result(resultString,
																		ArrayRef<char>(),
																		resultPoints,
																		BarcodeFormat::UPC_EAN_EXTENSION));
		}
		
		int UPCEANExtension5Reader::decodeMiddle(Ref<BitArray> row,
																						 int rangeStart,
																						 int rangeEnd,																						
																						 std::string& resultString)
		{
			vector<int>& counters (decodeMiddleCounters);
			int end = row->getSize();
			int rowOffset = rangeStart;
			int lgPatternFound = 0;
			int digitSize = 0;
			for (int x = 0; x < 5 && rowOffset < end; x++) {
				int bestMatch = -1;
				bestMatch = UPCEANReader::decodeDigit(row, counters, rowOffset, UPCEANReader::L_AND_G_PATTERNS);
				char theChar = (char) ('0' + bestMatch % 10);
				
				stringstream strstr;
				string str;
				
				strstr << theChar;
				strstr >> str;
				
				resultString.append(str);
				
				digitSize = rowOffset;
				for (int i = 0; i < (int)decodeMiddleCounters.size(); i++) {
					rowOffset += decodeMiddleCounters[i];
				}
				digitSize = (rowOffset - digitSize) >> 1;
				
				if (bestMatch >= 10) {
					lgPatternFound |= 1 << (4 - x);
				}
				
				if (x != 4) {
					// Read off separator if not last
					rowOffset = row->getNextSet(rowOffset);
					rowOffset = row->getNextUnset(rowOffset);
				}
				
			}
	
			if (resultString.length() == 5) {
				// Use check digit
				int checkDigit = determineCheckDigit(lgPatternFound);
				if (extensionChecksum(resultString) != checkDigit) {
					throw ChecksumException();
				}
			} else {
				throw NotFoundException();
			}
			
			// Check for (half a digit) tailing whitespace
			int quietEnd = rowOffset + digitSize;
			if (quietEnd >= row->getSize() || !row->isRange(rowOffset, quietEnd, false)) {
				throw NotFoundException();
			}
			
			return rowOffset;
		}
		
		int UPCEANExtension5Reader::determineCheckDigit(int lgPatternFound)
		{
			for (int d = 0; d < 10; d++) {
				if (lgPatternFound == CHECK_DIGIT_ENCODINGS[d]) {
					return d;
				}
			}
			return -1;
		}
		
		int UPCEANExtension5Reader::extensionChecksum(std::string s) {
			int length = s.length();
			int sum = 0;
			for (int i = length - 2; i >= 0; i -= 2) {
				sum += (int) s.at(i) - (int) '0';
			}
			sum *= 3;
			for (int i = length - 1; i >= 0; i -= 2) {
				sum += (int) s.at(i) - (int) '0';
			}
			sum *= 3;
			return sum % 10;
		}
	}
}