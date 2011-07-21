#ifndef MACRO_H
#define MACRO_H

/* Macro to write less code */
#define PROPERTY(type, name) \
public: \
    inline type& name() { return m_prop_##name;}\
    inline const type &name() const { return m_prop_##name;} \
private: \
    type m_prop_##name;\
public:

#define SETVEC3FCOORDS(vec, x, y, z) \
    inline float x() const {return vec[0];} \
    inline float y() const {return vec[1];} \
    inline float z() const {return vec[2];} \
    inline float& x() {return vec[0];} \
    inline float& y() {return vec[1];} \
    inline float& z() {return vec[2];}


#endif // MACRO_H
