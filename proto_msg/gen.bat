DEL *.c
DEL *.h
cd proto_gen
DEL *.proto
cd ..
COPY "MBMessage.proto" "proto_gen/MBMessage.proto"
cd proto_gen
protoc.exe --nanopb_out=.. *.proto