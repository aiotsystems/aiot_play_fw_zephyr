# ***********************************************************************
# FILENAME :        main.c
#
# DESCRIPTION :
#      Script that listens to port 1880 where the JsonServer posts.
#      We grab the payload of Memfault chunks and push them to the cloud
#      by using the Memfault CLI.
#
#
# NOTES :
#      This application is part of the OpenSwarm Project.
#
#      Copyright Siemens AG and Inria.  All rights reserved.
#
# AUTHOR :    Fabian Graf       START DATE :    09 Oct 2023
#
# CHANGES :
#
# VERSION DATE    WHO     DETAIL
# 0       09Oct23 FG      Initial Commit
#
#

import http.server
import os
import json

# Create an empty list to store incoming HTTP messages
http_messages = []

# Settings
project_key = "***************************"

# Define a custom request handler that captures incoming requests
class RequestHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        # Capture the incoming HTTP message for GET requests
        message = self.rfile.read(int(self.headers['Content-Length'])).decode('utf-8')
        http_messages.append(message)

    def do_POST(self):
        # Capture the incoming HTTP message for POST requests
        content_length = int(self.headers['Content-Length'])
        message = self.rfile.read(content_length).decode('utf-8')
        http_messages.append(message)
        print(message)
        json_object = json.loads(message)
        raw = json_object['fields']['data']
        hex_list = []
        for symbol in raw:
            hex_symbol = hex(symbol)[2:].zfill(2)
            hex_list.append(hex_symbol)
        hex_string = ''.join(hex_list)
        post_command = "memfault --project-key " + project_key + " post-chunk --encoding hex " + hex_string
        os.system(command=post_command)


def main():
    # Set up the HTTP server on localhost and port 1880
    server_address = ('localhost', 1880)
    httpd = http.server.HTTPServer(server_address, RequestHandler)

    print("Listening for HTTP messages on http://localhost:1880...")

    try:
        # Start the server
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")

    # Print the captured HTTP messages
    print("\nCaptured HTTP messages:")
    for idx, message in enumerate(http_messages, start=1):
        print(f"Message {idx}:\n{message}\n")


if __name__ == "__main__":
    main()
