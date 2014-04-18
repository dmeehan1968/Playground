//
//  main.cpp
//  Messaging
//
//  Created by Dave Meehan on 18/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>

#include <zmq.h>

#include <ZingBDD/ZingBDD.h>

int main(int argc, const char * argv[])
{
	ZingBDD::StreamReporter reporter(std::cout);
	ZingBDD::Runner::getInstance().run(reporter);
	
    return 0;
}

