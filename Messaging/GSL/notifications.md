## The Notifications Protocol

NB: This files is 100% generated and will be recreated
at the next build.  Any modifications WILL BE LOST!

A TAG consists of a multipart message of 5
frames:

| Frame # | Field | Type |
| ------------- | ------------- | ---------- |
|1|Message Identifier|"WRS/1.0/TAG"|
|2|Reader Serial| Integer |
|3|Server Timestamp| ISO 8601 |
|4|First Seen Time (seconds since Reader boot)| Integer |
|5|Last Seen Time (seconds since Reader boot)| Integer |
A HEARTBEAT consists of a multipart message of 9
frames:

| Frame # | Field | Type |
| ------------- | ------------- | ---------- |
|1|Message Identifier|"WRS/1.0/HEARTBEAT"|
|2|Reader Serial| Integer |
|3|Server Timestamp| ISO 8601 |
|4|Heartbeat interval| Integer |
|5|Auto Report| Boolean |
|6|Receiver Gain| Integer |
|7|Relay 0| Boolean |
|8|Relay 1| Boolean |
|9|Has Tags| Boolean |
