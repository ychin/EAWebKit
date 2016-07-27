/*
Copyright (C) 2004, 2009, 2010, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EATextSquish.cpp
//
// By Sumedha Kshirsagar, Paul Pedriana
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// To do: Replace with an optimized DXT encoder.
//
///////////////////////////////////////////////////////////////////////////////

   
#include <EAText/internal/EATextSquish.h>
#include <EASTL/algorithm.h>
#include <math.h>


#ifndef FLT_EPSILON
    #define FLT_EPSILON 1.192092896e-07f // smallest such that 1.0+FLT_EPSILON != 1.0
#endif
#ifndef FLT_MAX
    #define FLT_MAX     3.402823466e+38f // max floating point value
#endif


namespace EATextSquish
{


class Vec3
{
public:
    typedef Vec3 const& Arg;

    Vec3()
    {}

    explicit Vec3( float a )
    {
        m_x = a;
        m_y = a;
        m_z = a;
    }

    Vec3( float a, float b, float c )
    {
        m_x = a;
        m_y = b;
        m_z = c;
    }
    
    float X() const { return m_x; }
    float Y() const { return m_y; }
    float Z() const { return m_z; }
    
    Vec3 operator-() const
    {
        return Vec3( -m_x, -m_y, -m_z );
    }
    
    Vec3& operator+=( Arg v )
    {
        m_x += v.m_x;
        m_y += v.m_y;
        m_z += v.m_z;
        return *this;
    }
    
    Vec3& operator-=( Arg v )
    {
        m_x -= v.m_x;
        m_y -= v.m_y;
        m_z -= v.m_z;
        return *this;
    }
    
    Vec3& operator*=( Arg v )
    {
        m_x *= v.m_x;
        m_y *= v.m_y;
        m_z *= v.m_z;
        return *this;
    }
    
    Vec3& operator*=( float s )
    {
        m_x *= s;
        m_y *= s;
        m_z *= s;
        return *this;
    }
    
    Vec3& operator/=( Arg v )
    {
        m_x /= v.m_x;
        m_y /= v.m_y;
        m_z /= v.m_z;
        return *this;
    }
    
    Vec3& operator/=( float s )
    {
        float t = 1.0f/s;
        m_x *= t;
        m_y *= t;
        m_z *= t;
        return *this;
    }
    
    friend Vec3 operator+( Arg left, Arg right )
    {
        Vec3 copy( left );
        return copy += right;
    }
    
    friend Vec3 operator-( Arg left, Arg right )
    {
        Vec3 copy( left );
        return copy -= right;
    }
    
    friend Vec3 operator*( Arg left, Arg right )
    {
        Vec3 copy( left );
        return copy *= right;
    }
    
    friend Vec3 operator*( Arg left, float right )
    {
        Vec3 copy( left );
        return copy *= right;
    }
    
    friend Vec3 operator*( float left, Arg right )
    {
        Vec3 copy( right );
        return copy *= left;
    }
    
    friend Vec3 operator/( Arg left, Arg right )
    {
        Vec3 copy( left );
        return copy /= right;
    }
    
    friend Vec3 operator/( Arg left, float right )
    {
        Vec3 copy( left );
        return copy /= right;
    }
    
    friend float Dot( Arg left, Arg right )
    {
        return left.m_x*right.m_x + left.m_y*right.m_y + left.m_z*right.m_z;
    }
    
    friend Vec3 Min( Arg left, Arg right )
    {
        return Vec3(
            eastl::min_alt( left.m_x, right.m_x ), 
            eastl::min_alt( left.m_y, right.m_y ), 
            eastl::min_alt( left.m_z, right.m_z )
        );
    }

    friend Vec3 Max( Arg left, Arg right )
    {
        return Vec3(
            eastl::max_alt( left.m_x, right.m_x ), 
            eastl::max_alt( left.m_y, right.m_y ), 
            eastl::max_alt( left.m_z, right.m_z )
        );
    }

    friend Vec3 Floor( Arg v )
    {
        return Vec3(
            (float)floor( v.m_x ), 
            (float)floor( v.m_y ), 
            (float)floor( v.m_z )
        );
    }

private:
    float m_x;
    float m_y;
    float m_z;
};


inline float LengthSquared( Vec3::Arg v )
{
    return Dot( v, v );
}


class Sym3x3
{
public:
    Sym3x3()
    {}

    Sym3x3( float a )
    {
        for( int i = 0; i < 6; ++i )
            m_x[i] = a;
    }

    float operator[]( int index ) const
    {
        return m_x[index];
    }

    float& operator[]( int index )
    {
        return m_x[index];
    }

private:
    float m_x[6];
};


class ColorSet
{
public:
    ColorSet( const uint8_t* pARGB, uint32_t nSourceStride, uint32_t nImageSize);

    int          GetCount() const { return m_count; }
    const Vec3*  GetPoints() const { return m_points; }
    const float* GetWeights() const { return m_weights; }
    bool         IsTransparent() const { return m_transparent; }

    void RemapIndices( const uint8_t* source, uint8_t* target ) const;

private:
    int     m_count;
    Vec3    m_points[16];
    float   m_weights[16];
    int     m_remap[16];
    bool    m_transparent;
};



class ColourFit
{
public:
    ColourFit( const ColorSet* colours);
    virtual ~ColourFit() { }

    void Compress( void* block );

protected:
    virtual void Compress4( void* block ) = 0;

    const ColorSet* m_colours;
    int m_flags;
};



class ClusterFit : public ColourFit
{
public:
    ClusterFit( ColorSet const* colours);

    virtual void Compress4( void* block );

private:
    void  Reorder( Vec3::Arg principle );
    float SolveLeastSquares( Vec3& start, Vec3& end ) const;

    Vec3    m_principle;
    Vec3    m_weighted[16];
    Vec3    m_unweighted[16];
    float   m_weights[16];
    Vec3    m_metric;
    float   m_alpha[16];
    float   m_beta[16];
    Vec3    m_xxsum;
    float   m_besterror;
    int     m_order[16];
};


struct SourceBlock
{
    uint8_t start;
    uint8_t end;
    uint8_t error;
};


struct SingleColourLookup
{
    SourceBlock sources[4];
};


class SingleColourFit : public ColourFit
{
public:
    SingleColourFit( ColorSet const* colours);

    virtual void Compress4( void* block ); 
   
private:
    void ComputeEndPoints( int count, SingleColourLookup const* const* lookups );
    
    uint8_t m_colour[3];
    Vec3    m_start;
    Vec3    m_end;
    uint8_t m_index;
    int     m_error;
    int     m_besterror;
};

Sym3x3 ComputeWeightedCovariance( int n, Vec3 const* points, float const* weights );
Vec3 ComputePrincipleComponent( Sym3x3 const& matrix );


static int FloatToInt( float a, int limit )
{
    // use ANSI round-to-zero behaviour to get round-to-nearest
    int i = ( int )( a + 0.5f );

    // clamp to the limit
    if( i < 0 )
        i = 0;
    else if( i > limit )
        i = limit; 

    // done
    return i;
}


static int FloatTo565( Vec3::Arg colour )
{
    // get the components in the correct range
    const int r = FloatToInt(31.f * colour.X(), 31);
    const int g = FloatToInt(63.f * colour.Y(), 63);
    const int b = FloatToInt(31.f * colour.Z(), 31);
    
    // pack into a single value
    return ( r << 11 ) | ( g << 5 ) | b;
}

static void WriteColourBlock(int c0, int c1, uint8_t* pIndexes, void* pBlock)
{
    uint8_t* const pBlock8 = (uint8_t*)pBlock;

    // Write the two colors.
    #if   defined(EA_SYSTEM_BIG_ENDIAN)
        pBlock8[0] = (uint8_t)(c0 >>    8);
        pBlock8[1] = (uint8_t)(c0  & 0xff);
        pBlock8[2] = (uint8_t)(c1 >>    8);
        pBlock8[3] = (uint8_t)(c1  & 0xff);
    #else
        pBlock8[0] = (uint8_t)(c0  & 0xff);
        pBlock8[1] = (uint8_t)(c0 >>    8);
        pBlock8[2] = (uint8_t)(c1  & 0xff);
        pBlock8[3] = (uint8_t)(c1 >>    8);
    #endif

    // Write the color indexes.
    // The DXT format specifies that the indexes are two uint16_t successive 
    // uint16_t values. So we need to deal with the endian-ness of these values.
    #if   defined(EA_SYSTEM_BIG_ENDIAN)
        const uint8_t* pI  = pIndexes;
        uint16_t*      p16 = (uint16_t*)pBlock + 2;

        *p16++ = (pI[0]  << 0) | (pI[1]  <<  2) | (pI[2]  <<  4) | (pI[3]  <<  6) | 
                 (pI[4]  << 8) | (pI[5]  << 10) | (pI[6]  << 12) | (pI[7]  << 14);

        *p16   = (pI[8]  << 0) | (pI[9]  <<  2) | (pI[10] <<  4) | (pI[11] <<  6) | 
                 (pI[12] << 8) | (pI[13] << 10) | (pI[14] << 12) | (pI[15] << 14);
    #else
        for(int i = 0; i < 4; ++i)
        {
            const uint8_t* const pI = pIndexes + (4 * i);
            pBlock8[4 + i] = pI[0] | (pI[1] << 2) | (pI[2] << 4) | (pI[3] << 6);
        }
    #endif
}


void WriteColourBlock4(Vec3::Arg start, Vec3::Arg end, uint8_t const* pIndexes, void* block)
{
    int c0 = FloatTo565(start);
    int c1 = FloatTo565(end);

    // Possibly remap the indexes
    uint8_t remapped[16];

    if(c0 < c1) // This isn't actually necessary for DXT2+, as it ignores the color order. But DXT1 uses color order.
    {
        eastl::swap(c0, c1);

        for(int i = 0; i < 16; ++i)
            remapped[i] =  (pIndexes[i] ^ 0x1) & 0x3;
    }
    else if(c0 == c1) // If the colors are identical, just always use one of them.
    {
        for(int i = 0; i < 16; ++i)
            remapped[i] = 0;
    }
    else
    {
        for(int i = 0; i < 16; ++i)
            remapped[i] = pIndexes[i];
    }
    
    WriteColourBlock(c0, c1, remapped, block);
}


Sym3x3 ComputeWeightedCovariance( int n, Vec3 const* points, float const* weights )
{
    // compute the centroid
    float total = 0.0f;
    Vec3 centroid( 0.0f );
    for( int i = 0; i < n; ++i )
    {
        total += weights[i];
        centroid += weights[i]*points[i];
    }
    centroid /= total;

    // accumulate the covariance matrix
    Sym3x3 covariance( 0.0f );
    for( int i = 0; i < n; ++i )
    {
        Vec3 a = points[i] - centroid;
        Vec3 b = weights[i]*a;
        
        covariance[0] += a.X()*b.X();
        covariance[1] += a.X()*b.Y();
        covariance[2] += a.X()*b.Z();
        covariance[3] += a.Y()*b.Y();
        covariance[4] += a.Y()*b.Z();
        covariance[5] += a.Z()*b.Z();
    }
    
    // return it
    return covariance;
}

static Vec3 GetMultiplicity1Evector( Sym3x3 const& matrix, float evalue )
{
    // compute M
    Sym3x3 m;
    m[0] = matrix[0] - evalue;
    m[1] = matrix[1];
    m[2] = matrix[2];
    m[3] = matrix[3] - evalue;
    m[4] = matrix[4];
    m[5] = matrix[5] - evalue;

    // compute U
    Sym3x3 u;
    u[0] = m[3]*m[5] - m[4]*m[4];
    u[1] = m[2]*m[4] - m[1]*m[5];
    u[2] = m[1]*m[4] - m[2]*m[3];
    u[3] = m[0]*m[5] - m[2]*m[2];
    u[4] = m[1]*m[2] - m[4]*m[0];
    u[5] = m[0]*m[3] - m[1]*m[1];

    // find the largest component
    float mc = (float)fabs( u[0] );
    int mi = 0;
    for( int i = 1; i < 6; ++i )
    {
        float c = (float)fabs( u[i] );
        if( c > mc )
        {
            mc = c;
            mi = i;
        }
    }

    // pick the column with this component
    switch( mi )
    {
        case 0:
            return Vec3( u[0], u[1], u[2] );

        case 1:
        case 3:
            return Vec3( u[1], u[3], u[4] );

        default:
            return Vec3( u[2], u[4], u[5] );
    }
}


static Vec3 GetMultiplicity2Evector( Sym3x3 const& matrix, float evalue )
{
    // compute M
    Sym3x3 m;

    m[0] = matrix[0] - evalue;
    m[1] = matrix[1];
    m[2] = matrix[2];
    m[3] = matrix[3] - evalue;
    m[4] = matrix[4];
    m[5] = matrix[5] - evalue;

    // find the largest component
    float mc = (float)fabs( m[0] );
    int mi = 0;
    for( int i = 1; i < 6; ++i )
    {
        float c = (float)fabs( m[i] );
        if( c > mc )
        {
            mc = c;
            mi = i;
        }
    }

    // pick the first eigenvector based on this index
    switch( mi )
    {
        case 0:
        case 1:
            return Vec3( -m[1], m[0], 0.0f );

        case 2:
            return Vec3( m[2], 0.0f, -m[0] );

        case 3:
        case 4:
            return Vec3( 0.0f, -m[4], m[3] );

        default:
            return Vec3( 0.0f, -m[5], m[4] );
    }
}


Vec3 ComputePrincipleComponent( Sym3x3 const& matrix )
{
    // compute the cubic coefficients
    float c0 = matrix[0]*matrix[3]*matrix[5] 
        + 2.0f*matrix[1]*matrix[2]*matrix[4] 
        - matrix[0]*matrix[4]*matrix[4] 
        - matrix[3]*matrix[2]*matrix[2] 
        - matrix[5]*matrix[1]*matrix[1];
    float c1 = matrix[0]*matrix[3] + matrix[0]*matrix[5] + matrix[3]*matrix[5]
        - matrix[1]*matrix[1] - matrix[2]*matrix[2] - matrix[4]*matrix[4];
    float c2 = matrix[0] + matrix[3] + matrix[5];

    // compute the quadratic coefficients
    float a = c1 - ( 1.0f/3.0f )*c2*c2;
    float b = ( -2.0f/27.0f )*c2*c2*c2 + ( 1.0f/3.0f )*c1*c2 - c0;

    // compute the root count check
    float Q = 0.25f*b*b + ( 1.0f/27.0f )*a*a*a;

    // test the multiplicity
    if( FLT_EPSILON < Q )
    {
        // only one root, which implies we have a multiple of the identity
        return Vec3( 1.0f );
    }
    else if( Q < -FLT_EPSILON )
    {
        // three distinct roots
        float theta = (float)atan2( sqrt( -Q ), -0.5f*b );
        float rho = (float)sqrt( 0.25f*b*b - Q );

        float rt = (float)pow( rho, 1.0f/3.0f );
        float ct = (float)cos( theta/3.0f );
        float st = (float)sin( theta/3.0f );

        float l1 = ( 1.0f/3.0f )*c2 + 2.0f*rt*ct;
        float l2 = ( 1.0f/3.0f )*c2 - rt*( ct + ( float )sqrt( 3.0f )*st );
        float l3 = ( 1.0f/3.0f )*c2 - rt*( ct - ( float )sqrt( 3.0f )*st );

        // pick the larger
        if( fabs( l2 ) > fabs( l1 ) )
            l1 = l2;
        if( fabs( l3 ) > fabs( l1 ) )
            l1 = l3;

        // get the eigenvector
        return GetMultiplicity1Evector( matrix, l1 );
    }
    else // if( -FLT_EPSILON <= Q && Q <= FLT_EPSILON )
    {
        // two roots
        float rt;
        if( b < 0.0f )
            rt = (float)-pow( -0.5f*b, 1.0f/3.0f );
        else
            rt = (float) pow( 0.5f*b, 1.0f/3.0f );
        
        float l1 = ( 1.0f/3.0f )*c2 + rt;        // repeated
        float l2 = ( 1.0f/3.0f )*c2 - 2.0f*rt;
        
        // get the eigenvector
        if( fabs( l1 ) > fabs( l2 ) )
            return GetMultiplicity2Evector( matrix, l1 );
        else
            return GetMultiplicity1Evector( matrix, l2 );
    }
}


#include "SingleColorLookup.inl"

SingleColourFit::SingleColourFit( ColorSet const* colours)
  : ColourFit( colours)
{
    // grab the single colour
    Vec3 const* values = m_colours->GetPoints();
    m_colour[0] = ( uint8_t )FloatToInt( 255.0f*values->X(), 255 );
    m_colour[1] = ( uint8_t )FloatToInt( 255.0f*values->Y(), 255 );
    m_colour[2] = ( uint8_t )FloatToInt( 255.0f*values->Z(), 255 );
        
    // initialise the best error
    m_besterror = INT_MAX;
}


void SingleColourFit::Compress4( void* block )
{
    // build the table of lookups
    SingleColourLookup const* const lookups[] = 
    {
        lookup_5_4, 
        lookup_6_4, 
        lookup_5_4
    };
    
    // find the best end-points and index
    ComputeEndPoints( 4, lookups );
    
    // build the block if we win
    if( m_error < m_besterror )
    {
        // remap the indices
        uint8_t indices[16];
        m_colours->RemapIndices( &m_index, indices );
        
        // save the block
        WriteColourBlock4( m_start, m_end, indices, block );

        // save the error
        m_besterror = m_error;
    }
}

void SingleColourFit::ComputeEndPoints( int count, SingleColourLookup const* const* lookups )
{
    // check each index combination
    m_error = INT_MAX;
    for( int index = 0; index < count; ++index )
    {
        // check the error for this codebook index
        SourceBlock const* sources[3];
        int error = 0;
        for( int channel = 0; channel < 3; ++channel )
        {
            // grab the lookup table and index for this channel
            SingleColourLookup const* lookup = lookups[channel];
            int target = m_colour[channel];
            
            // store a pointer to the source for this channel
            sources[channel] = lookup[target].sources + index;
            
            // accumulate the error
            int diff = sources[channel]->error;
            error += diff*diff;            
        }
        
        // keep it if the error is lower
        if( error < m_error )
        {
            m_start = Vec3(
                ( float )sources[0]->start/31.0f, 
                ( float )sources[1]->start/63.0f, 
                ( float )sources[2]->start/31.0f
            );
            m_end = Vec3(
                ( float )sources[0]->end/31.0f, 
                ( float )sources[1]->end/63.0f, 
                ( float )sources[2]->end/31.0f
            );
            m_index = ( uint8_t )index;
            m_error = error;
        }
    }
}

ColorSet::ColorSet(uint8_t const* pARGB, uint32_t nSourceStride, uint32_t nImageSize)
  : m_count(0), 
    m_transparent(false)
{
    #if defined(EA_SYSTEM_BIG_ENDIAN)
        const int kA = 0;
        const int kR = 1;
        const int kG = 2;
        const int kB = 3;
    #else
        const int kA = 3;
        const int kR = 2;
        const int kG = 1;
        const int kB = 0;
    #endif

  //const bool isDxt1        = false;
    const bool weightByAlpha = false;  // Note by Paul Pedriana: I think this would be better if it was true.

    // Note by Paul Pedriana: Sumedha implemented clipping here, but I think we could 
    // find an alternative way to clip the pixels without copying data like this.
    uint8_t argb[64];

    for(int row = 0; row < 4; ++row)
    {
        for(int col=0; col<16; col++)
            argb[(row * 16) + col] = (((row * nSourceStride) + col) < (nImageSize * 4)) ? pARGB[(row * nSourceStride) + col] : 0x00000000;
    }

    for(int i = 0; i < 16; ++i)
    {
        // loop over previous points for a match
        for(int j = 0; ; ++j)
        {
            if(j == i) // If we've checked all the previous colors without finding a match...
            {
                // Convert argb values from [0,255] to [0,1]
                const float b = (float)(argb[4*i + kB]    ) / 255.0f;
                const float g = (float)(argb[4*i + kG]    ) / 255.0f;
                const float r = (float)(argb[4*i + kR]    ) / 255.0f;
                const float a = (float)(argb[4*i + kA] + 1) / 256.0f; // Ensure non-zero weight for any alpha.

                // add the point
                m_points[m_count]  = Vec3(r, g, b);
                m_weights[m_count] = (weightByAlpha ? a : 1.0f);
                m_remap[i]         = m_count;
                
                ++m_count;
                break;
            }
        
            // See if this color matches the current color. 
            const bool match = (argb[4*i + kB] == argb[4*j + kB])
                            && (argb[4*i + kG] == argb[4*j + kG])
                            && (argb[4*i + kR] == argb[4*j + kR])
                            /*&& (argb[4*j + kA] || !isDxt1)*/;  // isDxt1 is always false (see above) so this line is always true and thus need not exist.

            if( match )
            {
                const int   index = m_remap[j];                           // Get the index of the match.
                const float a     = (float)(argb[4*i + kA] + 1) / 256.0f; // Ensure non-zero weight for any alpha.

                // Map to this point and increase the weight.
                m_weights[index] += (weightByAlpha ? a : 1.0f);
                m_remap[i] = index;
                break;
            }
        }
    }
    
}

void ColorSet::RemapIndices( uint8_t const* source, uint8_t* target ) const
{
    for( int i = 0; i < 16; ++i )
    {
        const int j = m_remap[i];

        if( j == -1 )
            target[i] = 3;
        else
            target[i] = source[j];
    }
}

ClusterFit::ClusterFit( ColorSet const* colours) 
  : ColourFit( colours )
{
    // initialise the best error
    m_besterror = FLT_MAX;

    // initialise the metric
    m_metric = Vec3( 0.2126f, 0.7152f, 0.0722f );

    // cache some values
    int const count = m_colours->GetCount();
    Vec3 const* values = m_colours->GetPoints();
    
    // get the covariance matrix
    Sym3x3 covariance = ComputeWeightedCovariance( count, values, m_colours->GetWeights() );
    
    // compute the principle component
    Vec3 principle = ComputePrincipleComponent( covariance );

    // build the list of values
    float dps[16];
    float next = FLT_MAX;
    for( int i = 0; i < count; ++i )
    {
        dps[i] = Dot( values[i], principle );
        if( dps[i] < next )
            next = dps[i];
    }
        
    // stable sort
    for( int n = 0; n < count; )
    {
        float current = next;
        next = FLT_MAX;
        for( int i = 0; i < count; ++i )
        {
            if( dps[i] == current )
                m_order[n++] = i;
            else if( current < dps[i] && dps[i] < next )
                next = dps[i];
        }
    }

    // weight all the points
    Vec3 const* unweighted = m_colours->GetPoints();
    float const* weights = m_colours->GetWeights();
    m_xxsum = Vec3( 0.0f );

    for( int i = 0; i < count; ++i )
    {
        int p = m_order[i];
        m_unweighted[i] = unweighted[p];
        m_weights[i] = weights[p];
        m_weighted[i] = weights[p]*unweighted[p];
        m_xxsum += m_weighted[i]*m_weighted[i];
    }
}

void ClusterFit::Compress4( void* block )
{
    // declare variables
    int const count = m_colours->GetCount();
    Vec3 beststart( 0.0f );
    Vec3 bestend( 0.0f );
    float besterror = m_besterror;
    float const twothirds = 2.0f/3.0f;
    float const onethird = 1.0f/3.0f;
    float const zero = 0.0f;

    // check all possible clusters for this total order
    uint8_t indices[16];
    uint8_t bestindices[16];
    
    // first cluster [0,i) is at the start
    for( int m = 0; m < count; ++m )
    {
        indices[m] = 0;
        m_alpha[m] = m_weights[m];
        m_beta[m] = zero;
    }
    for( int i = count; i >= 0; --i )
    {
        // second cluster [i,j) is one third along
        for( int m = i; m < count; ++m )
        {
            indices[m] = 2;
            m_alpha[m] = twothirds*m_weights[m];
            m_beta[m] = onethird*m_weights[m];
        }        
        for( int j = count; j >= i; --j )
        {
            // third cluster [j,k) is two thirds along
            for( int m = j; m < count; ++m )
            {
                indices[m] = 3;
                m_alpha[m] = onethird*m_weights[m];
                m_beta[m] = twothirds*m_weights[m];
            }        
            for( int k = count; k >= j; --k )
            {
                // last cluster [k,n) is at the end
                if( k < count )
                {
                    indices[k] = 1;
                    m_alpha[k] = zero;
                    m_beta[k] = m_weights[k];
                }                
                
                // solve a least squares problem to place the endpoints
                Vec3 start, end;
                float error = SolveLeastSquares( start, end );

                // keep the solution if it wins
                if( error < besterror )
                {
                    beststart = start;
                    bestend = end;
                    for( int m = 0; m < 16; ++m )    // TODO: make this faster?
                        bestindices[m] = indices[m];    
                    besterror = error;
                }
            }
        }
    }

    // save the block if necessary
    if( besterror < m_besterror )
    {
        // remap the indices
        uint8_t unordered[16];
        for( int i = 0; i < count; ++i )
            unordered[m_order[i]] = bestindices[i];
        m_colours->RemapIndices( unordered, bestindices );
        
        // save the block
        WriteColourBlock4( beststart, bestend, bestindices, block );

        // save the error
        m_besterror = besterror;
    }
}

float ClusterFit::SolveLeastSquares( Vec3& start, Vec3& end ) const
{
    // accumulate all the quantities we need
    int const count = m_colours->GetCount();
    float alpha2_sum = 0.0f;
    float beta2_sum = 0.0f;
    float alphabeta_sum = 0.0f;
    Vec3 alphax_sum( 0.0f );
    Vec3 betax_sum( 0.0f );    
    for( int i = 0; i < count; ++i )
    {
        float alpha = m_alpha[i];
        float beta = m_beta[i];
        Vec3 const& x = m_weighted[i];
        
        alpha2_sum += alpha*alpha;
        beta2_sum += beta*beta;
        alphabeta_sum += alpha*beta;
        alphax_sum += alpha*x;
        betax_sum += beta*x;
    }

    // zero where non-determinate
    Vec3 a, b;
    if( beta2_sum == 0.0f )
    {
        a = alphax_sum/alpha2_sum;
        b = Vec3( 0.0f );
    }
    else if( alpha2_sum == 0.0f )
    {
        a = Vec3( 0.0f );
        b = betax_sum/beta2_sum;
    }
    else
    {
        float factor = 1.0f/( alpha2_sum*beta2_sum - alphabeta_sum*alphabeta_sum );
        
        a = ( alphax_sum*beta2_sum - betax_sum*alphabeta_sum )*factor;
        b = ( betax_sum*alpha2_sum - alphax_sum*alphabeta_sum )*factor;
    }
    
    // clamp the output to [0, 1]
    Vec3 const one( 1.0f );
    Vec3 const zero( 0.0f );
    a = Min( one, Max( zero, a ) );
    b = Min( one, Max( zero, b ) );

    // clamp to the grid
    Vec3 const grid( 31.0f, 63.0f, 31.0f );
    Vec3 const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f );
    Vec3 const half( 0.5f );
    a = Floor( grid*a + half )*gridrcp;
    b = Floor( grid*b + half )*gridrcp;

    // compute the error
    Vec3 e1 = a*a*alpha2_sum + b*b*beta2_sum + m_xxsum
        + 2.0f*( a*b*alphabeta_sum - a*alphax_sum - b*betax_sum );

    // apply the metric to the error term
    float error = Dot( e1, m_metric );
    
    // save the start and end
    start = a;
    end = b;
    return error;
}

ColourFit::ColourFit( ColorSet const* colours) 
  : m_colours( colours )
{
}

void ColourFit::Compress( void* block )
{
    Compress4( block );
}


void Compress(const uint8_t* pSourceARGB, void* pDestination, uint32_t nSourceStride, uint32_t nImageSize)
{
    // A DXT3 block consists of 8 bytes of alpha info followed by 
    // 8 bytes of color info. This function writes only the color info.
    void* const pDestinationColor = (uint8_t*)pDestination + 8;

    ColorSet colorSet(pSourceARGB, nSourceStride, nImageSize);
    
    if(colorSet.GetCount() == 1) // If the block is monochrome...
    {
        SingleColourFit fit(&colorSet);
        fit.Compress4(pDestinationColor);
    }
    else
    {
        ClusterFit fit(&colorSet);
        fit.Compress4(pDestinationColor);
    }
}


} // namespace EATextSquish














