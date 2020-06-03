#include "net_util.h"

unsigned char NetUtil::checksum(char buf[], int size) {
	unsigned char checksum = 0x02;

	for (int i = 0; i < size; i++) {
		checksum ^= buf[i];
	}
	checksum ^= 0x03;
	return checksum;
}