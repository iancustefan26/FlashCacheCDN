chmod +x install_dependencies.sh
chmod +x set_config.sh
chmod +x spawn_egde_servers.sh

sudo ./install_dependencies.sh

./spawn_egde_servers.sh

echo "Look at your wanted machine's IP for the EDNS server"
./set_config.sh

echo "\n\nModify the src/EDNS_server/config/edge_servers_info.json at line 2 with the desired IP address for the edge server and then run\n mkdir build\n cd build \n cmake .. \n make"
