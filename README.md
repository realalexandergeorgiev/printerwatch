# printerwatch
Windows program to monitor printer queue written in C.

Works by sending a query to the server, retrieving hostname, username, document name and number of pages from pool server. Output can be piped into a textfile for further processing. 

Can be interesting in enterprise environments where only one pool server is in use. I have used it in a pentest. 

Note that the documents itself cannot be retrieved - only the document title and the user who printed it. Also it can only querry the queue, not the printed documents. Thats why it tries to query the server multiple times.

# compilation
Compiles without errors in codeblocks (link against Winspool.lib)
