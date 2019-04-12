mkdir grpc_src
cd grpc_src
git init
git remote add -f origin https://github.com/grpc/grpc
git config core.sparseCheckout true
echo "examples/cpp/helloworld" >> .git/info/sparse-checkout
git pull origin master