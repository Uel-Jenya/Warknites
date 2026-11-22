namespace renderer{
    class Base{
        public:
            Base();
            virtual ~Base();
            Base(const Base&) = delete;
            Base(Base&&) = delete;
            Base& operator = (const Base&) = delete;
            Base& operator = (const Base&&) = delete;
    };
}