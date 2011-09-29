#!/bin/bash
good=0
wrong=0
blu="good guess!"
for i in {1..100}
do
  result=`./client 130.237.218.85 12321 STANDALONE practice`
  if [ "$result" == "$blu" ];
  then
    let "good += 1"

  else
    let "wrong += 1"
  fi
done

echo "good : $good wrong : $wrong"
