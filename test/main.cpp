#include <gtest/gtest.h>

#include <Common/Base/hkBase.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}