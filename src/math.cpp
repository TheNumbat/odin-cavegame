
#pragma push_macro("sqrtf")
#pragma push_macro("tanf")
#pragma push_macro("abs")
#pragma push_macro("absf")
#pragma push_macro("roundf")
#pragma push_macro("ceilf")
#pragma push_macro("floorf")
#pragma push_macro("sinf")
#pragma push_macro("cosf")
#pragma push_macro("lerpf")

#undef sqrtf
#undef tanf
#undef abs
#undef absf
#undef roundf
#undef ceilf
#undef floorf
#undef sinf
#undef cosf

inline u32 mod(u32 dividend, u32 devisor) { FUNC
	return dividend % devisor;
}

inline f32 _sqrtf(f32 value) { FUNC
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(value)));
}

inline f32 _tanf(f32 value) { FUNC
	return tanf(value);
}

inline i32 _abs(i32 value) { FUNC
	return value > 0 ? value : -value;
}

inline f32 absf_(f32 value) { FUNC
	return value > 0.0f ? value : -value;	
}

inline f64 absf_(f64 value) { FUNC
	return value > 0.0f ? value : -value;	
}

inline f32 _roundf(f32 value) { FUNC
	return roundf(value);
}

inline f32 _ceilf(f32 value) { FUNC
	return ceilf(value);
}

inline f32 _floorf(f32 value) { FUNC
	return (f32)(i32)value;
}

inline f32 _sinf(f32 value) { FUNC
	return sinf(value);
}

inline f32 _cosf(f32 value) { FUNC
	return cosf(value);
}

inline f32 _lerpf(f32 min, f32 max, f32 dist) { FUNC
	return min + (max - min) * dist;
}

#pragma pop_macro("sqrtf")
#pragma pop_macro("tanf")
#pragma pop_macro("abs")
#pragma pop_macro("absf")
#pragma pop_macro("roundf")
#pragma pop_macro("ceilf")
#pragma pop_macro("floorf")
#pragma pop_macro("sinf")
#pragma pop_macro("cosf")
#pragma pop_macro("lerpf")

template<typename T> inline v2_t<T> V2(T x, T y) { FUNC
	v2_t<T> ret;
	ret.x = x;
	ret.y = y;
	return ret;
}
template v2 V2(f32 x, f32 y);
template<typename T> inline v2 V2f(T x, T y) { FUNC
	return V2((f32)x, (f32)y);
}
template<typename T> inline uv2 V2u(T x, T y) { FUNC
	return V2((u32)x, (u32)y);
}

template<typename T> inline v3_t<T> V3(T x, T y, T z) { FUNC
	v3_t<T> ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	return ret;
}
template v3 V3(f32 x, f32 y, f32 z);
template<typename T> inline v3 V3f(T x, T y, T z) { FUNC
	return V3((f32)x, (f32)y, (f32)z);
}
template<typename T> inline uv3 V3u(T x, T y, T z) { FUNC
	return V3((u32)x, (u32)y, (u32)z);
}
template<typename T> inline bv3 V3b(T x, T y, T z) { FUNC
	return V3((u8)x, (u8)y, (u8)z);
}

template<typename T> inline v4_t<T> V4(T x, T y, T z, T w) { FUNC
	v4_t<T> ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	ret.w = w;
	return ret;
}
template v4 V4(f32 x, f32 y, f32 z, f32 w);
template<typename T> inline bv4 V4b(T x, T y, T z, T w) { FUNC
	return V4((u8)x, (u8)y, (u8)z, (u8)w);
}
template<typename T> inline bv4 V4b(bv3 vec3, T w) { FUNC
	return V4b(vec3.x, vec3.y, vec3.z, (u8)w);
}
inline v4 color_to_f(color c) { FUNC
	return V4(c.x / 255.0f, c.y / 255.0f, c.z / 255.0f, c.w / 255.0f);
}

const color WHITE = V4b(255, 255, 255, 255);
const color BLACK = V4b(0, 0, 0, 255);
const color RED   = V4b(255, 0, 0, 255);
const color GREEN = V4b(0, 255, 0, 255);
const color BLUE  = V4b(0, 0, 255, 255);

template<typename T> inline T clamp(T V, T min, T max) { FUNC
	if(V < min) return min;
	if(V > max) return max;
	return V;
}
template f32 clamp(f32, f32, f32);
template i32 clamp(i32, i32, i32);
template<typename T> inline v2_t<T> clamp(v2_t<T> V, T min, T max) { FUNC
	return V2(clamp(V.x, min, max), clamp(V.y, min, max));
}
template v2 clamp(v2, f32, f32);
template<typename T> inline v3_t<T> clamp(v3_t<T> V, T min, T max) { FUNC
	return V3(clamp(V.x, min, max), clamp(V.y, min, max), clamp(V.z, min, max));
}
template v3 clamp(v3, f32, f32);
template<typename T> inline v4_t<T> clamp(v4_t<T> V, T min, T max) { FUNC
	return V4(clamp(V.x, min, max), clamp(V.y, min, max), clamp(V.z, min, max), clamp(V.w, min, max));
}
template v4 clamp(v4, f32, f32);

template<typename T> inline T lengthsq(v2_t<T> V) { FUNC
	return V.x * V.x + V.y * V.y;
}
template f32 lengthsq(v2);
template<typename T> inline T lengthsq(v3_t<T> V) { FUNC
	return V.x * V.x + V.y * V.y + V.z * V.z;
}
template f32 lengthsq(v3);
template<typename T> inline T lengthsq(v4_t<T> V) { FUNC
	return V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w;
}
template f32 lengthsq(v4);

template<typename T> inline T length(v2_t<T> V) { FUNC
	return sqrtf(lengthsq(V));
}
template f32 length(v2);
template<typename T> inline T length(v3_t<T> V) { FUNC
	return sqrtf(lengthsq(V));
}
template f32 length(v3);
template<typename T> inline T length(v4_t<T> V) { FUNC
	return sqrtf(lengthsq(V));
}
template f32 length(v4);

template<typename T> inline v2_t<T> normalize(v2_t<T> V) { FUNC
	T len = length(V);
	return V2(V.x * ((T)1.0 / len), V.y * ((T)1.0 / len));
}
template v2 normalize(v2);
template<typename T> inline v3_t<T> normalize(v3_t<T> V) { FUNC
	T len = length(V);
	return V3(V.x * ((T)1.0 / len), V.y * ((T)1.0 / len), V.z * ((T)1.0 / len));
}
template v3 normalize(v3);
template<typename T> inline v4_t<T> normalize(v4_t<T> V) { FUNC
	T len = length(V);
	return V4(V.x * ((T)1.0 / len), V.y * ((T)1.0 / len), V.z * ((T)1.0 / len), V.w * ((T)1.0 / len));
}
template v4 normalize(v4);

template<typename T> inline T dot(v2_t<T> l, v2_t<T> r) { FUNC
	return l.x * r.x + l.y * r.y;
}
template f32 dot(v2, v2);
template<typename T> inline T dot(v3_t<T> l, v3_t<T> r) { FUNC
	return l.x * r.x + l.y * r.y + l.z * r.z;
}
template f32 dot(v3, v3);
template<typename T> inline T dot(v4_t<T> l, v4_t<T> r) { FUNC
	return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}
template f32 dot(v4, v4);

template<typename T> inline v3_t<T> cross(v3_t<T> l, v3_t<T> r) { FUNC
	return V3(l.y * r.z - l.z * r.y, l.z * r.x - l.x * r.z, l.x * r.y - l.y * r.x);
}
template v3 cross(v3, v3);

template<typename T> inline v2_t<T> add(v2_t<T> l, v2_t<T> r) { FUNC
	return V2(l.x + r.x, l.y + r.y);
}
template v2 add(v2, v2);
template<typename T> inline v3_t<T> add(v3_t<T> l, v3_t<T> r) { FUNC
	return V3(l.x + r.x, l.y + r.y, l.z + r.z);
}
template v3 add(v3, v3);
template<typename T> inline v4_t<T> add(v4_t<T> l, v4_t<T> r) { FUNC
	return V4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);	
}
template v4 add(v4, v4);

template<typename T> inline v2_t<T> sub(v2_t<T> l, v2_t<T> r) { FUNC
	return V2(l.x - r.x, l.y - r.y);
}
template v2 sub(v2, v2);
template<typename T> inline v3_t<T> sub(v3_t<T> l, v3_t<T> r) { FUNC
	return V3(l.x - r.x, l.y - r.y, l.z - r.z);
}
template v3 sub(v3, v3);
template<typename T> inline v4_t<T> sub(v4_t<T> l, v4_t<T> r) { FUNC
	return V4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);	
}
template v4 sub(v4, v4);

template<typename T> inline v2_t<T> mult(v2_t<T> l, v2_t<T> r) { FUNC
	return V2(l.x * r.x, l.y * r.y);
}
template v2 mult(v2, v2);
template<typename T> inline v2_t<T> mult(v2_t<T> l, T r) { FUNC
	return V2(l.x * r, l.y * r);
}
template v2 mult(v2, f32);
template<typename T> inline v3_t<T> mult(v3_t<T> l, v3_t<T> r) { FUNC
	return V3(l.x * r.x, l.y * r.y, l.z * r.z);
}
template v3 mult(v3, v3);
template<typename T> inline v3_t<T> mult(v3_t<T> l, T r) { FUNC
	return V3(l.x * r, l.y * r, l.z * r);
}
template v3 mult(v3, f32);
template<typename T> inline v4_t<T> mult(v4_t<T> l, v4_t<T> r) { FUNC
	return V4(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w);
}
template v4 mult(v4, v4);
template<typename T> inline v4_t<T> mult(v4_t<T> l, T r) { FUNC
	return V4(l.x * r, l.y * r, l.z * r, l.w * r);
}
template v4 mult(v4, f32);

template<typename T> inline v2_t<T> div(v2_t<T> l, v2_t<T> r) { FUNC
	return V2(l.x / r.x, l.y / r.y);
}
template v2 div(v2, v2);
template<typename T> inline v2_t<T> div(v2_t<T> l, T r) { FUNC
	return V2(l.x / r, l.y / r);
}
template v2 div(v2, f32);
template<typename T> inline v3_t<T> div(v3_t<T> l, v3_t<T> r) { FUNC
	return V3(l.x / r.x, l.y / r.y, l.z / r.z);
}
template v3 div(v3, v3);
template<typename T> inline v3_t<T> div(v3_t<T> l, T r) { FUNC
	return V3(l.x / r, l.y / r, l.z / r);
}
template v3 div(v3, f32);
template<typename T> inline v4_t<T> div(v4_t<T> l, v4_t<T> r) { FUNC
	return V4(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w);
}
template v4 div(v4, v4);
template<typename T> inline v4_t<T> div(v4_t<T> l, T r) { FUNC
	return V4(l.x / r, l.y / r, l.z / r, l.w / r);
}
template v4 div(v4, f32);

template<typename T> inline r2_t<T> R2(T x, T y, T w, T h) { FUNC
	r2_t<T> ret;
	ret.x = x;
	ret.y = y;
	ret.w = w;
	ret.h = h;
	return ret;
}
template r2 R2(f32, f32, f32, f32);
r2 R2f(i32 x, i32 y, i32 w, i32 h) { FUNC
	return R2((f32)x, (f32)y, (f32)w, (f32)h);
}
template<typename T> inline r2_t<T> R2(v2_t<T> xy, v2_t<T> wh) { FUNC
	return R2(xy.x, xy.y, wh.x, wh.y);
}
template r2 R2(v2, v2); 

template<typename T> inline r2_t<T> add(r2_t<T> l, r2_t<T> r) { FUNC
	return R2(l.x + r.x, l.y + r.y, l.w + r.w, l.h + r.h);	
}
template r2 add(r2, r2);
template<typename T> inline r2_t<T> sub(r2_t<T> l, r2_t<T> r) { FUNC
	return R2(l.x - r.x, l.y - r.y, l.w - r.w, l.h - r.h);	
}
template r2 sub(r2, r2);

template<typename T> inline r2_t<T> mult(r2_t<T> r, T f) { FUNC
	return R2(r.x, r.y, r.w * f, r.h * f);
}
template r2 mult(r2, f32);

template<typename T> inline bool inside(r2_t<T> r, T x, T y) { FUNC
	return (x >= r.x) &&
		   (x <= r.x + r.w) &&
		   (y >= r.y) &&
		   (y <= r.y + r.h);
}
template bool inside(r2, f32, f32);
template<typename T> inline bool inside(r2_t<T> r, v2_t<T> v) { FUNC
	return inside(r, v.x, v.y);
}
template bool inside(r2, v2);


// TODO(max): more SIMD
// Several of these matrix algorithms adapted from https://github.com/StrangeZak/Handmade-Math
template<typename T> inline m4_t<T> M4D(T diag) { FUNC
	m4_t<T> ret;
	ret._11 = diag;
	ret._22 = diag;
	ret._33 = diag;
	ret._44 = diag;
	return ret;
}
template m4 M4D(f32);

template<typename T> inline m4_t<T> add(m4_t<T> l, m4_t<T> r) { FUNC
	m4_t<T> ret;
	ret._11 = l._11 + r._11; ret._12 = l._12 + r._12; ret._13 = l._13 + r._13; ret._14 = l._14 + r._14;
	ret._21 = l._21 + r._21; ret._22 = l._22 + r._22; ret._23 = l._23 + r._23; ret._24 = l._24 + r._24;
	ret._31 = l._31 + r._31; ret._32 = l._32 + r._32; ret._33 = l._33 + r._33; ret._34 = l._34 + r._34;
	ret._41 = l._41 + r._41; ret._42 = l._42 + r._42; ret._43 = l._43 + r._43; ret._44 = l._44 + r._44;
	return ret;
}
template m4 add(m4, m4);

template<typename T> inline m4_t<T> sub(m4_t<T> l, m4_t<T> r) { FUNC
	m4_t<T> ret;
	ret._11 = l._11 - r._11; ret._12 = l._12 - r._12; ret._13 = l._13 - r._13; ret._14 = l._14 - r._14;
	ret._21 = l._21 - r._21; ret._22 = l._22 - r._22; ret._23 = l._23 - r._23; ret._24 = l._24 - r._24;
	ret._31 = l._31 - r._31; ret._32 = l._32 - r._32; ret._33 = l._33 - r._33; ret._34 = l._34 - r._34;
	ret._41 = l._41 - r._41; ret._42 = l._42 - r._42; ret._43 = l._43 - r._43; ret._44 = l._44 - r._44;
	return ret;
}
template m4 sub(m4, m4);

template<typename T> m4_t<T> mult(m4_t<T> l, m4_t<T> r) { FUNC
	m4_t<T> ret;
    for(i32 col = 0; col < 4; col++) {
	    for(i32 row = 0; row < 4; row++) {
            T sum = 0;
            for(i32 place = 0; place < 4; place++) {
                sum += l.f[col][place] * r.f[place][row];
            }
            ret.f[col][row] = sum;
        }
    }
    return ret;
}

template<> inline m4 mult(m4 l, m4 r) { FUNC
    m4 ret;
    __m128 row1 = _mm_load_ps(&r.v[0]);
    __m128 row2 = _mm_load_ps(&r.v[4]);
    __m128 row3 = _mm_load_ps(&r.v[8]);
    __m128 row4 = _mm_load_ps(&r.v[12]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(l.v[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(l.v[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(l.v[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(l.v[4*i + 3]);
        __m128 row = _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                    _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&ret.v[4*i], row);
    }
    return ret;
}

template<typename T> inline m4_t<T> mult(m4_t<T> m, T s) { FUNC
	m4_t<T> ret;
	ret._11 = m._11 * s; ret._12 = m._12 * s; ret._13 = m._13 * s; ret._14 = m._14 * s;
	ret._21 = m._21 * s; ret._22 = m._22 * s; ret._23 = m._23 * s; ret._24 = m._24 * s;
	ret._31 = m._31 * s; ret._32 = m._32 * s; ret._33 = m._33 * s; ret._34 = m._34 * s;
	ret._41 = m._41 * s; ret._42 = m._42 * s; ret._43 = m._43 * s; ret._44 = m._44 * s;
	return ret;
}
template m4 mult(m4, f32);

template<typename T> inline v4_t<T> mult(m4_t<T> m, v4_t<T> v) { FUNC
    v4_t<T> ret;
    for(i32 row = 0; row < 4; row++) {
        T sum = 0;
        for(i32 col = 0; col < 4; col++) {
            sum += m.f[row][col] * v.f[col];
        }
        ret.f[row] = sum;
    }
    return ret;
}
template v4 mult(m4, v4);

template<typename T> inline m4_t<T> div(m4_t<T> m, T s) { FUNC
	m4_t<T> ret;
	ret._11 = m._11 / s; ret._12 = m._12 / s; ret._13 = m._13 / s; ret._14 = m._14 / s;
	ret._21 = m._21 / s; ret._22 = m._22 / s; ret._23 = m._23 / s; ret._24 = m._24 / s;
	ret._31 = m._31 / s; ret._32 = m._32 / s; ret._33 = m._33 / s; ret._34 = m._34 / s;
	ret._41 = m._41 / s; ret._42 = m._42 / s; ret._43 = m._43 / s; ret._44 = m._44 / s;
	return ret;
}
template m4 div(m4, f32);

template<typename T> inline m4_t<T> transpose(m4_t<T> m) { FUNC
	m4_t<T> ret;
	ret._11 = m._11; ret._12 = m._21; ret._13 = m._31; ret._14 = m._41;
	ret._21 = m._12; ret._22 = m._22; ret._23 = m._32; ret._24 = m._42;
	ret._31 = m._13; ret._32 = m._23; ret._33 = m._33; ret._34 = m._43;
	ret._41 = m._14; ret._42 = m._24; ret._43 = m._34; ret._44 = m._44;
	return ret;
}
template m4 transpose(m4);

inline m4 ortho(f32 left, f32 right, f32 bot, f32 top, f32 _near, f32 _far) { FUNC
    m4 ret;
    ret.f[0][0] = 2.0f / (right - left);
    ret.f[1][1] = 2.0f / (top - bot);
    ret.f[2][2] = 2.0f / (_near - _far);
    ret.f[3][3] = 1.0f;
    ret.f[3][0] = (-left - right) / (right - left);
    ret.f[3][1] = (-bot - top)  / (top - bot);
    ret.f[3][2] = - _near / (_far - _near);
    return ret;
}

inline m4 proj(f32 fov, f32 ar, f32 _near, f32 _far) { FUNC
    m4 ret = M4D(1.0f);
    f32 tan_over_2 = tanf(RADIANS(fov) / 2.0f);
    ret.f[1][1] = 1.0f / tan_over_2;
    ret.f[0][0] = ret.f[1][1] / ar;
    ret.f[2][2] = -_far / (_far - _near);
    ret.f[2][3] = -1.0f;
    ret.f[3][2] = 2.0f * (-_far * _near) / (_far - _near);
    ret.f[3][3] = 0.0f;
    return ret;
}

inline m4 translate(v3 trans) { FUNC
	m4 ret = M4D(1.0f);
    ret.f[3][0] = trans.x;
    ret.f[3][1] = trans.y;
    ret.f[3][2] = trans.z;
    return ret;
}

inline m4 rotate(f32 angle, v3 axis) { FUNC

	m4 ret;

	f32 c = cosf(RADIANS(angle));
	f32 s = sinf(RADIANS(angle));

	axis = normalize(axis);
	v3 temp = mult(axis, (1 - c));

	ret.f[0][0] = c + temp.f[0] * axis.f[0];
	ret.f[0][1] = temp.f[0] * axis.f[1] + s * axis.f[2];
	ret.f[0][2] = temp.f[0] * axis.f[2] - s * axis.f[1];
	ret.f[1][0] = temp.f[1] * axis.f[0] - s * axis.f[2];
	ret.f[1][1] = c + temp.f[1] * axis.f[1];
	ret.f[1][2] = temp.f[1] * axis.f[2] + s * axis.f[0];
	ret.f[2][0] = temp.f[2] * axis.f[0] + s * axis.f[1];
	ret.f[2][1] = temp.f[2] * axis.f[1] - s * axis.f[0];
	ret.f[2][2] = c + temp.f[2] * axis.f[2];

	return ret;
}

inline m4 scale(v3 scale) { FUNC
    m4 ret = M4D(1.0f);
    ret.f[0][0] = scale.x;
    ret.f[1][1] = scale.y;
    ret.f[2][2] = scale.z;
    return ret;
}

inline m4 lookAt(v3 eye, v3 center, v3 up) { FUNC
    m4 ret = M4D(0.0f);

    v3 F = normalize(sub(center, eye));
    v3 S = normalize(cross(F, up));
    v3 U = cross(S, F);

    ret.f[0][0] =  S.x;
    ret.f[0][1] =  U.x;
    ret.f[0][2] = -F.x;

    ret.f[1][0] =  S.y;
    ret.f[1][1] =  U.y;
    ret.f[1][2] = -F.y;

    ret.f[2][0] =  S.z;
    ret.f[2][1] =  U.z;
    ret.f[2][2] = -F.z;

    ret.f[3][0] = -dot(S, eye);
    ret.f[3][1] = -dot(U, eye);
    ret.f[3][2] =  dot(F, eye);
    ret.f[3][3] = 1.0f;

    return ret;
}