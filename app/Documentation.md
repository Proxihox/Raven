# Documentation on server.cpp

This file is used to launch the server.

It first creates the MOQT object using `MOQT()`.

Then the server starts listening on the local `QUIC_ADDRESS` using `start_listener`

An assertion check is done to ensure that the server is in a valid state before proceeding.

This is followed by seeting up a unique registration and configuration for this server with the help of the unique_handler1 and unique_handler2 respectively.

We then create a unique_listener using unique_handler2, which will listen for any clients.