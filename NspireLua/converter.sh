#!/bin/bash
set +eu

cat LuaNspire_before.lua NspTEST.lua LuaNspire_after.lua > big.lua
luasrcdiet big.lua -o big_opt.lua 1>/dev/null
(echo -n 'LUAS'; cat big_opt.lua;) | convbin -i /dev/stdin -o "LuaTEST.8xv" -j bin -k 8xv -r -n LuaTEST
