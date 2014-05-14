
# NomProtocol

This is an example development based on the ZeroMQ NOM protocol (http://zguide.zeromq.org/page:all#Using-ABNF) as the basis.

It also uses GSL (http://github.com/imatix/gsl) and is partially based on ZPROTO (http://github.com/zeromq/zproto).

## Goal

To generate C++11 sources from protocol definitions written in XML and processed using the GSL interpretter, applying GSL scripts that output well formed and valid code.

## About the Example

There are stylistic differences in the XML used to describe the protocol, and there are minor differences in the definition of the protocol for the sake of the enhancing the examples.

Here is the original ABNF from the guide:

    nom-protocol    = open-peering *use-peering

    open-peering    = C:OHAI ( S:OHAI-OK / S:WTF )

    use-peering     = C:ICANHAZ
                    / S:CHEEZBURGER
                    / C:HUGZ S:HUGZ-OK
                    / S:HUGZ C:HUGZ-OK

The above is designed to describe the packets that are permitted in each of the states, but I think its syntatically wrong. 'open-peering' suggests that the client sends OHAI, and the server responds with either OHAI-OK or WTF.  So far so good.

Once we are in 'use-peering' state its more ambiguous.  It might be right in terms of strict operator precedence, but I think what is intended is:

* The client can send ICANHAZ and the server responds CHEEZBURGER

* The client can send HUGZ and the server responds HUGZ-OK

* The server can send HUGZ and the client responds HUGZ-OK

Additional parentheses make this clearer, and I've added a response in the case that the server can't provide cheeze burgers:

    nom-protocol    =   open-peering *use-peering

    open-peering    =   C:OHAI ( S:OHAI-OK / S:WTF )

    use-peering     =   ( C:ICANHAZ ( S:CHEEZBURGER / S:WTF ) )
                    /=  ( C:HUGZ S:HUGZ-OK )
                    /=  ( S:HUGZ C:HUGZ-OK )

So this is going to be the basis of this example.

## XML Protocol Definition

`NomProtocol.xml` contains the XML definition of the protocol based on the ABNF.  I like ABNF, but its not sufficiently feature-full as to allow us to actually describe a protocol, its really designed as a character stream parser, for which it was designed.





