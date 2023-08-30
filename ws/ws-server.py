#!/usr/bin/env python
# https://github.com/Pithikos/python-websocket-server
# python3 -m pip install websocket-server

import logging
from websocket_server import WebsocketServer

def new_client(client, server):
	print("new client connected and was given id {}".format(client['id']))

def client_left(client, server):
	print("client({}) disconnected".format(client['id']))

def message_received(client, server, message):
	print("client({}) said: {}".format(client['id'], message))
	responce = "ok"
	# server.send_message_to_all(responce)
	server.send_message(client, responce)

server = WebsocketServer(host='0.0.0.0', port=8080, loglevel=logging.INFO)
server.set_fn_new_client(new_client)
server.set_fn_client_left(client_left)
server.set_fn_message_received(message_received)
server.run_forever()
