cd src/build
cmake ../
make -j32
cp src ../../app
docker-compose build
docker-compose up -d
