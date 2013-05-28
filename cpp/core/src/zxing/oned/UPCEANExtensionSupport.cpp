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

#include <zxing/oned/UPCEANExtension5Reader.h>
#include <zxing/oned/UPCEANExtension2Reader.h>
#include <zxing/oned/UPCEANExtensionSupport.h>
#include <zxing/NotFoundException.h>
#include <zxing/oned/UPCEANReader.h>

using namespace std;

namespace zxing {
	namespace oned {
		// Variables
		static const int EXTENSION_START_PATTERN_LENGTH = 3;
		static const int _EXTENSION_START_PATTERN[EXTENSION_START_PATTERN_LENGTH] = {1,1,2};
		
#define VECTOR_INIT(v) v, v + sizeof(v)/sizeof(v[0])
		const vector<int> UPCEANExtensionSupport::EXTENSION_START_PATTERN(VECTOR_INIT(_EXTENSION_START_PATTERN));
		
		UPCEANExtension2Reader *extension2Reader;
		UPCEANExtension5Reader *extension5Reader;
		
		// Methods
		UPCEANExtensionSupport::UPCEANExtensionSupport() {
			extension2Reader = new UPCEANExtension2Reader::UPCEANExtension2Reader();
			extension5Reader = new UPCEANExtension5Reader::UPCEANExtension5Reader();
		}
		
		Ref<Result> UPCEANExtensionSupport::decodeRow(int rowNumber, Ref<BitArray> row, int rangeStart, int rangeEnd) {
			UPCEANReader::Range startPattern = UPCEANReader::findGuardPattern(row, rangeStart, false, EXTENSION_START_PATTERN);
			Ref<Result> extensionResult;
			try {
				extensionResult = extension5Reader->decodeRow(rowNumber, row, startPattern[1], row->getSize());
			} catch (NotFoundException exception) {
			}
			if (!extensionResult) {
				try {
				extensionResult = extension2Reader->decodeRow(rowNumber, row, startPattern[1], row->getSize());
				} catch (NotFoundException exception) {
					// continue
				}
			}
			if (extensionResult) {
				return extensionResult;
			} else {
				throw NotFoundException();
			}
		}
		
		UPCEANExtensionSupport::~UPCEANExtensionSupport() {
			delete extension2Reader;
			delete extension5Reader;
		}
	}
}