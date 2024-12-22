chmod +x src/install_build_scripts/install_dependencies.sh
chmod +x src/install_build_scripts/set_config.sh
chmod +x src/install_build_scripts/spawn_egde_servers.sh

sudo ./src/install_build_scripts/install_dependencies.sh

./src/install_build_scripts/spawn_egde_servers.sh

echo "Set your EDNS server IP"
echo ""
cd src/install_build_scripts

./set_config.sh

cd ../..

echo ""
echo ""

echo ""
echo ""
echo "mkdir build" 
echo "cd build"
echo "cmake .."
echo "make"

echo "Run the server, EDNS server, edge servers and then the client. Test by using ./(test/test_5_seconds/intese_load/find_primes) available resources from /src/edge_server/available_resources"

echo "./test prints Hello World!"
echo "./test prints Hello World! after 5 seconds of sleep"
echo "./find_primes print the prime numbers up to 100"
echo "./intense_load triggers an infinite loop on the chosen machine"

echo "P.S: The project directory is mounted to the edge-servers (so all the edge-server will have all the resources available). If you want to test what happens if you request a resource that is not available on all the edge-servers you will have to manually copy the file directory to a VM or another machine and create your own scripts there in src/edge-servers/available_resources that will be autamatically compiled at build time."

echo ""

echo "Well, what will happen is that you will be redirected to a server that will have the resource already cached. Told you for saving your testing time :)"
