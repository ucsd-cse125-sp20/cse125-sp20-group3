#pragma once

class NetUtil {
public:
	static unsigned char checksum(char buf[], int size);
};