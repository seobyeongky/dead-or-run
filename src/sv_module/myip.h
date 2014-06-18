#pragma once

struct IPv4
{
    unsigned char b1, b2, b3, b4;
};

bool getMyIP(IPv4 & myIP);