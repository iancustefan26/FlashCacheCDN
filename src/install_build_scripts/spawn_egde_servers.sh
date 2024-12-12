#!/bin/bash

if ! snap list | grep -q multipass; then
    echo "Installing Multipass..."
    sudo snap install multipass
else
    echo "Multipass is already installed."
fi

echo "Locating the project directory..."
PROJECT_NAME="FlashCacheCDN"
PROJECT_PATH=$(find ~ -type d -name "$PROJECT_NAME" 2>/dev/null)

if [ -z "$PROJECT_PATH" ]; then
    echo "Error: Project directory '$PROJECT_NAME' not found."
    exit 1
fi
echo "Project directory located: $PROJECT_PATH"

# Create and configure instances
for i in {1..5}; do
    INSTANCE_NAME="edge-server-$i"
    echo "Launching instance $INSTANCE_NAME..."
    multipass launch -n "$INSTANCE_NAME" --cpus 1 --memory 2G --disk 5G
    
    if [ $? -ne 0 ]; then
        echo "Error: Failed to launch $INSTANCE_NAME."
        continue
    fi

    echo "Mounting project directory to $INSTANCE_NAME..."
    multipass mount "$PROJECT_PATH" "$INSTANCE_NAME:/home/ubuntu/$PROJECT_NAME"
    
    if [ $? -eq 0 ]; then
        echo "Project directory successfully mounted to $INSTANCE_NAME."
    else
        echo "Error: Failed to mount project directory to $INSTANCE_NAME."
    fi

    echo "Installing dependencies on $INSTANCE_NAME"
    multipass exec "$INSTANCE_NAME" -- exec /home/ubuntu/$PROJECT_NAME/install_dependencies.sh

    if [ $? -eq 0 ]; then
        echo "Dependencies successfully installed to $INSTANCE_NAME"
    else
        echo "Error: Failed to install dependencies to $INSTANCE_NAME"
    fi
done

echo "All instances launched and configured."

