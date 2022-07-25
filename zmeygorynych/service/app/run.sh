#!/bin/sh
socat tcp-l:42069,reuseaddr,fork EXEC:"/app/src",pty,echo=0
