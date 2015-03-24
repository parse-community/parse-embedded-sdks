#!/bin/bash

# build yun docs
doxygen parse_yun_doc_config

#build main docs
doxygen parse_doc_config
