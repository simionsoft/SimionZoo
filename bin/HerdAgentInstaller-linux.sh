HERD_AGENT_INSTALL_DIR=/usr/bin/herd-agent
HERD_AGENT_TEMP_DIR=$HERD_AGENT_INSTALL_DIR/temp
LOCK=/tmp/herd-agent
DAEMON_INIT_DIR=/etc/init.d
DAEMON_INIT_FILE=herd-agent-daemon

if [ -f $DAEMON_INIT_DIR/$DAEMON_INIT_FILE ]; then
  if [ -f $LOCK ]; then
    sudo $DAEMON_INIT_DIR/$DAEMON_INIT_FILE stop
  fi

  sudo rm -r $HERD_AGENT_INSTALL_DIR
  sudo mkdir $HERD_AGENT_INSTALL_DIR
  sudo mkdir $HERD_AGENT_TEMP_DIR
fi

echo "Installing dependecies..."
sudo apt-get install mono-complete

echo "Copying files to installation dir..."
sudo cp Herd.dll $HERD_AGENT_INSTALL_DIR
sudo cp HerdAgent.exe $HERD_AGENT_INSTALL_DIR
sudo chmod 770 $HERD_AGENT_INSTALL_DIR/HerdAgent.exe

echo "Installling daemon initialisation file..."
cp $DAEMON_INIT_FILE $DAEMON_INIT_DIR
sudo update-rc.d $DAEMON_INIT_FILE defaults

echo "Starting daemon..."
sudo $DAEMON_INIT_DIR/$DAEMON_INIT_FILE start
