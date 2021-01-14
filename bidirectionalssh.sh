#!/bin/sh

SSH_SERVER=`echo $2 | awk -F: '{print $1}'`
SSH_PORT=`echo $2 | awk -F: '{print $2}'`

if [ -z "$SSH_PORT" ]
then
  SSH_CMD="ssh"

else
  SSH_CMD="ssh -p $SSH_PORT"

fi

echo "Command is $SSH_CMD"

trap "exit" INT TERM
trap "kill 0" EXIT

# Redirect SSH input and output to temporary named pipes (FIFOs)
SSH_IN=$(mktemp -u)
SSH_OUT=$(mktemp -u)
mkfifo "$SSH_IN" "$SSH_OUT"
$SSH_CMD -tt "$SSH_SERVER" "$3" < "$SSH_IN" > "$SSH_OUT" &
#ssh "$SSH_SERVER" $3
#ssh "$SSH_SERVER" hostname

# Open the FIFO-s and clean up the files
exec 3>"$SSH_IN"
exec 4<"$SSH_OUT"
rm -f "$SSH_IN" "$SSH_OUT"

$1 <&4 >&3
