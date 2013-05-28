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

#include <zxing/oned/UPCEANExtension2Reader.h>
#include <zxing/NotFoundException.h>
#include <zxing/ChecksumException.h>
#include <zxing/oned/OneDResultPoint.h>
#include <zxing/oned/UPCEANReader.h>
#include <sstream>

using namespace std;

namespace zxing {
	namespace oned {
		UPCEANExtension2Reader::UPCEANExtension2Reader() : decodeMiddleCounters(4, 0) { }
		
		Ref<Result> UPCEANExtension2Reader::decodeRow(int rowNumber, Ref<BitArray> row, int rangeStart, int rangeEnd) {
			string& result = decodeRowStringBuffer;
			result.clear();
			int decodeOffset;
			try {
				decodeOffset = decodeMiddle(row, rangeStart, rangeEnd, result);
			} catch (ChecksumException exception) {
				throw NotFoundException();
			}
			Ref<String> resultString (new String(result));
			ArrayRef< Ref<ResultPoint> > resultPoints(2);
			resultPoints[0] = Ref<OneDResultPoint>(new OneDResultPoint(rangeStart, rowNumber));
			resultPoints[1] =  Ref<OneDResultPoint>(new OneDResultPoint(decodeOffset, rowNumber));
			return Ref<Result>(new Result(resultString,
																		ArrayRef<char>(),
																		resultPoints,
																		BarcodeFormat::UPC_EAN_EXTENSION));
		}
		
		int UPCEANExtension2Reader::decodeMiddle(Ref<BitArray> row,
																						 int rangeStart,
																						 int rangeEnd,
																						 std::string& resultString)
		{
			vector<int>& counters (decodeMiddleCounters);		
			int end = row->getSize();
			int rowOffset = rangeStart;
			int checkParity = 0;
			int digitSize = 0;
			for (int x = 0; x < 2 && rowOffset < end; x++) {
				int bestMatch = UPCEANReader::decodeDigit(row, counters, rowOffset, UPCEANReader::L_AND_G_PATTERNS);
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
					checkParity |= 1 << (1 - x);
				}
				
				if (x != 1) {
					// Read off separator if not last
					rowOffset = row->getNextSet(rowOffset);
					rowOffset = row->getNextUnset(rowOffset);
				}
			}
			
			if (resultString.length() != 2) {
				throw NotFoundException();
			}
			
			// Check for (half a digit) tailing whitespace
			int quietEnd = rowOffset + digitSize;
			if (quietEnd >= row->getSize() || !row->isRange(rowOffset, quietEnd, false)) {
				throw NotFoundException();
			}
			
			int resultNumber;
			stringstream convert(resultString);
			
			if (!(convert >> resultNumber)) {
				resultNumber = 0;
			}
			
			if((resultNumber % 4) != checkParity) {
				throw ChecksumException();
			}

			return rowOffset;
		}
	}
}