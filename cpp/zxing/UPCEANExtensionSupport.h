// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __ZXingWidget__UPCEANExtensionSupport__
#define __ZXingWidget__UPCEANExtensionSupport__

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

#include <vector>
#include <zxing/common/Counted.h>
#include <zxing/Result.h>
#include <zxing/oned/OneDReader.h>
#include <zxing/oned/UPCEANExtension2Reader.h>
#include <zxing/oned/UPCEANExtension5Reader.h>

namespace zxing {
	namespace oned {
		class UPCEANExtensionSupport {	
		private:
			UPCEANExtension2Reader extension2Reader;
			UPCEANExtension5Reader extension5Reader;
		protected:
			static const std::vector<int> EXTENSION_START_PATTERN;
		public:
			Ref<Result> decodeRow(int rowNumber, Ref<BitArray> row, int rangeStart, int rangeEnd);
		};
	}
}

#endif /* defined(__ZXingWidget__UPCEANExtensionSupport__) */
