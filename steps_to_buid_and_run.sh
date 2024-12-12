chmod +x src/install_build_scripts/install_dependencies.sh
chmod +x src/install_build_scripts/set_config.sh
chmod +x src/install_build_scripts/spawn_egde_servers.sh

sudo ./src/install_build_scripts/install_dependencies.sh

./src/install_build_scripts/spawn_egde_servers.sh

echo "Look at your wanted machine's IP for the EDNS server"
./src/install_build_scripts/set_config.sh

echo ""
echo ""

echo "Modify the src/EDNS_server/config/edge_servers_info.json at line 2 with the desired IP address for the edge server and then run:"
echo ""
echo ""
echo "mkdir build" 
echo "cd build" echo "cmake .."
echo "make"

echo "Run the server, EDNS server, edge servers and then the client. Test by using ./(test/test_5_seconds/intese_load) from /src/test_resources"
