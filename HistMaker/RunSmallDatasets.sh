#!/bin/sh
RW=0
if [ $1 -eq 0 ]; then
  for i in {3..4}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi

if [ $1 -eq 1 ]; then
  for i in {5..10}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi

if [ $1 -eq 2 ]; then
  for i in {11..12}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi

if [ $1 -eq 3 ]; then
  for i in {13..21}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi

if [ $1 -eq 4 ]; then
  for i in {22..30}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi

if [ $1 -eq 5 ]; then
  for i in {31..39}
  do
    root -q "MakeHistValidation.cc+(3,${i},-1,${RW})"
  done
fi