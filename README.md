# What is admute?
A full on-memory ad muter for ad-tech products

# Version

admute alpha 0.1

# What is ad muter?
Internet user likes mute advertisement. Because they are very annoying.
The ad-tech products will save the data record which user has muted. (Facebook, Line, etc...)
The system in here, is ad muter.

# Problems
A huge number of advertisements are distributed everyday.
And, Users are muting a very large number of ads everyday, too.
Before ad-tech system delivery an ad, it will check the user's muted-ads database, and exclude the ads if it is in this database.
This operation consume huge resources (CPU and Memories).

# Solution
I'd like to build an full on-memory base ad-muter,
every muted-ad will be processed on memory with accurate struct size. This is good for fast processing.
It just support some simple operation, `set` and `check`, or `set` an same userid, different adid for Append.

You can pass data by tcp-ip, next version I will support upd for `set`, it will take much less cost on network.
tcp/ip shake hands and timewait is slow.

# Todo
1. Reloading data when shutdown the admute and restart it. save memory data in filebase with sub thread.
2. More performance...
3. Maintenance-free
4. Stability

# Usage
I prepare an sample client in src. You can use it in C. or write an XS module for perl, etc...
I also write an string parser for it. I will improvement it in next beta version.

you can:

1. run server
```
# bin/server
```

2. in another terminal
```
# bin/client
```

  * You can read the sample source in `client.c`, it is necessary to send a `SOCKET_OBJ` memory instance to socket, a little hard to use like this, or you can send a string command on port 5001, see next step.

3. or echo string by netcat
```
echo "set USERID ADID" | nc 127.0.0.1 5001
echo "check USERID ADID" | nc 127.0.0.1 5001
```
# Port
Server listen on `5000`, client send `struct streaming memory` to server directly in same port `5000`.
It is hard to use. so I prepare `5001` port just for string socket communication.
