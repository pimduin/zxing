// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __ZXingWidget__UPCEANExtension5Reader__
#define __ZXingWidget__UPCEANExtension5Reader__

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

#include <string>
#include <vector>
#include <zxing/common/Counted.h>
#include <zxing/common/BitArray.h>
#include <zxing/Result.h>

namespace zxing {
	namespace oned {
		class UPCEANExtension5Reader {
		private:
			std::string decodeRowStringBuffer;
			std::vector<int> decodeMiddleCounters;
			
		public:
			UPCEANExtension5Reader();
			
			Ref<Result> decodeRow(int rowNumber,
														Ref<BitArray> row,
														int rangeStart,
														int rangeEnd);
			int decodeMiddle(Ref<BitArray> row,
											 int rangeStart,
											 int rangeEnd,
											 std::string& resultString);
			static int determineCheckDigit(int lgPatternFound);
			static int extensionChecksum(std::string s);
		};
	}
}

#endif /* defined(__ZXingWidget__UPCEANExtension5Reader__) */
