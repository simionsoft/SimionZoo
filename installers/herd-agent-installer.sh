echo "Installing Herd Agent service..."

HERD_AGENT_INSTALL_DIR=/usr/bin/herd-agent
HERD_AGENT_TEMP_DIR=$HERD_AGENT_INSTALL_DIR/temp
LOCK=/tmp/herd-agent
DAEMON_INIT_DIR=/usr/bin/herd-agent
DAEMON_INIT_FILE=daemon
SERVICE_INSTALL_DIR=/etc/systemd/system
SERVICE_NAME=herd-agent
SERVICE_FILE=$SERVICE_NAME.service

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
sudo cp ../bin/Herd.dll $HERD_AGENT_INSTALL_DIR
sudo cp ../bin/HerdAgent.exe $HERD_AGENT_INSTALL_DIR
sudo chmod 770 $HERD_AGENT_INSTALL_DIR/HerdAgent.exe

echo "Installling daemon initialisation file..."
cp $DAEMON_INIT_FILE $DAEMON_INIT_DIR
sudo chmod 770 $DAEMON_INIT_DIR/$DAEMON_INIT_FILE
cp $SERVICE_FILE $SERVICE_INSTALL_DIR

echo "Starting daemon..."
sudo systemctl enable $SERVICE_NAME
sudo systemctl start $SERVICE_NAME

