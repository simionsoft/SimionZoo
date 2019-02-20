HERD_AGENT_INSTALL_DIR=/usr/bin/herd-agent
HERD_AGENT_TEMP_DIR=$HERD_AGENT_INSTALL_DIR/temp
LOCK=/tmp/herd-agent
DAEMON_INIT_DIR=/etc/init.d
DAEMON_INIT_FILE=herd-agent-daemon

if [ -f $DAEMON_INIT_DIR/$DAEMON_INIT_FILE ]; then
  if [ -f $LOCK ]; then
    $DAEMON_INIT_DIR/$DAEMON_INIT_FILE stop
  fi

  rm -r $HERD_AGENT_INSTALL_DIR
  mkdir $HERD_AGENT_INSTALL_DIR
  mkdir $HERD_AGENT_TEMP_DIR
fi

echo "Installing dependencies..."
apt-get install mono-complete

echo "Copying files to /usr/bin..."
cp Herd.dll $HERD_AGENT_INSTALL_DIR
cp HerdAgent.exe $HERD_AGENT_INSTALL_DIR

echo "Installing daemon initialisation file..."
cp $DAEMON_INIT_FILE $DAEMON_INIT_DIR
update-rc.d $DAEMON_INIT_FILE defaults

echo "Starting daemon"
$DAEMON_INIT_DIR/$DAEMON_INIT_FILE start