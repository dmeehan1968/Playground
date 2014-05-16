//
//  main.c
//  zproto-nom
//
//  Created by Dave Meehan on 16/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>

typedef int bool
;
#include "../include/nom_server.h"

int main(int argc, char *argv[]) {

    nom_server_test(1);
    
    return 0;

}