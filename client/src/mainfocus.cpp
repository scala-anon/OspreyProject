class OspreyClient {
    public: 
        OspreyClient(const std::string& server_address)
            : stub_(DpIngestionService::NewStub(grpc::CreateChannel(server_address, grpc::InsecureChannelCredtials()))){}
        void registerProvider(){
            RegisterProviderRequest providerReq;
            RegisterProviderResponse providerRes;

            providerReq = makeRegisterProviderRequest()
        }
        void ingestData(){

        }
}
