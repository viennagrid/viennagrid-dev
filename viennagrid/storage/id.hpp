#ifndef VIENNAGRID_STORAGE_ID_HPP
#define VIENNAGRID_STORAGE_ID_HPP


namespace viennagrid
{

    namespace storage
    {
     
        template<typename id_type__>
        class id_handler
        {
        public:
            typedef id_type__ id_type;
            
            id_handler() {}
            id_handler(const id_handler & rhs) : id_(rhs.id_) {}
            
            void id(id_type id) { id_ = id; }
            id_type id() const { return id_; }

        private:
            id_type id_;
        };
            

        
        template<typename id_type__>
        class id_compare
        {
        public:
            typedef id_type__ id_type;
            
            id_compare(id_type id__) : id(id__) {}
            
            template<typename type>
            bool operator() ( const type & to_compare )
            {
                return to_compare.id() == id;
            }
            
        private:
            id_type id;
        };
        
        namespace id
        {
        
            template<typename element_type, typename id_type>
            void set_id( element_type & element, id_type id )
            {
                element.id(id);
            }
            
            template<typename id_type> void set_id( bool & element, id_type id ) {}
            template<typename id_type> void set_id( char & element, id_type id ) {}
            template<typename id_type> void set_id( unsigned char & element, id_type id ) {}
            template<typename id_type> void set_id( short & element, id_type id ) {}
            template<typename id_type> void set_id( unsigned short & element, id_type id ) {}
            template<typename id_type> void set_id( int & element, id_type id ) {}
            template<typename id_type> void set_id( unsigned int & element, id_type id ) {}
            template<typename id_type> void set_id( long & element, id_type id ) {}
            template<typename id_type> void set_id( unsigned long & element, id_type id ) {}
            template<typename id_type> void set_id( float & element, id_type id ) {}
            template<typename id_type> void set_id( double & element, id_type id ) {}
            
        }
        
        
    }
    
}

#endif