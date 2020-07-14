
    /*
    --------------------------------------------------------
     * HFUN-MESH-EUCLIDEAN-3D: unstructured H(x) in E^3.
    --------------------------------------------------------
     *
     * This program may be freely redistributed under the
     * condition that the copyright notices (including this
     * entire header) are not removed, and no compensation
     * is received through use of the software.  Private,
     * research, and institutional use is free.  You may
     * distribute modified versions of this code UNDER THE
     * CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE
     * TO IT IN THE SAME FILE REMAIN UNDER COPYRIGHT OF THE
     * ORIGINAL AUTHOR, BOTH SOURCE AND OBJECT CODE ARE
     * MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR
     * NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution
     * of this code as part of a commercial system is
     * permissible ONLY BY DIRECT ARRANGEMENT WITH THE
     * AUTHOR.  (If you are not directly supplying this
     * code to a customer, and you are instead telling them
     * how they can obtain it for free, then you are not
     * required to make any arrangement with me.)
     *
     * Disclaimer:  Neither I nor: Columbia University, The
     * Massachusetts Institute of Technology, The
     * University of Sydney, nor The National Aeronautics
     * and Space Administration warrant this code in any
     * way whatsoever.  This code is provided "as-is" to be
     * used at your own risk.
     *
    --------------------------------------------------------
     *
     * Last updated: 25 April, 2020
     *
     * Copyright 2013-2020
     * Darren Engwirda
     * de2363@columbia.edu
     * https://github.com/dengwirda/
     *
    --------------------------------------------------------
     */

#   pragma once

#   ifndef __HFUN_MESH_EUCLIDEAN_3__
#   define __HFUN_MESH_EUCLIDEAN_3__

    namespace mesh {

    template <
    typename R ,
    typename I ,
    typename A = allocators::basic_alloc
             >
    class hfun_mesh_euclidean_3d
        : public hfun_base_kd <I, R>
    {
    public  :

    /*------------------------- euclidean size-fun in R^3 */

    typedef R                       real_type ;
    typedef I                       iptr_type ;
    typedef A                       allocator ;

    typedef hfun_mesh_euclidean_3d  <
            real_type ,
            iptr_type >             hfun_type ;

    typedef typename  hfun_base_kd  <
            iptr_type ,
            real_type >::hint_type  hint_type ;

    typedef containers::array   <
            real_type ,
            allocator >             real_list ;


    typedef mesh_complex_node_3<I, R>
                                    node_type ;

    typedef mesh_complex_edge_2<I>  edge_type ;
    typedef mesh_complex_tria_3<I>  tri3_type ;
    typedef mesh_complex_quad_4<I>  quad_type ;
    typedef mesh_complex_tria_4<I>  tri4_type ;
    typedef mesh_complex_hexa_8<I>  hexa_type ;
    typedef mesh_complex_wedg_6<I>  wedg_type ;
    typedef mesh_complex_pyra_5<I>  pyra_type ;

    typedef mesh::mesh_complex_3<
            node_type,
            edge_type,
            tri3_type, quad_type,
            tri4_type, hexa_type,
            wedg_type, pyra_type,
            allocator           >   mesh_type ;

    typedef geom_tree::aabb_node_base_k
                                    tree_node ;

    typedef geom_tree::aabb_item_rect_k <
                float,
            iptr_type,
            + 3                 >   tree_item ;

    typedef geom_tree::aabb_tree<
            tree_item,
            + 3 ,
            tree_node,
            allocator           >   tree_type ;

    typedef geom_tree::aabb_pred_node_3 <
                float,
            iptr_type           >   tree_pred ;

    public  :

    containers::
        fixed_array<real_type,3>   _bmin ;
    containers::
        fixed_array<real_type,3>   _bmax ;

    mesh_type                      _mesh ;
    tree_type                      _tree ;

    containers::array<
        real_type, allocator >     _hval ;

    containers::array<
        real_type, allocator >     _dhdx ;

    public  :

    /*
    --------------------------------------------------------
     * INIT-HFUN: make tree - check signs.
    --------------------------------------------------------
     */

    __normal_call void_type init (
        )
    {
        class tria_pred
            {
    /*-------------------- TRUE if tree should index tria */
            public  :
            __inline_call
                bool_type operator () (
                tri4_type const& _tdat
                ) const
            {   return _tdat.mark() >= 0 ;
            }
            } ;

    /*----------------------------- init. aabb at -+ inf. */
        for(auto _idim = 3; _idim-- != 0 ; )
        {
            this->_bmin[_idim] =
                +std::numeric_limits<
                    real_type>::infinity() ;

            this->_bmax[_idim] =
                -std::numeric_limits<
                    real_type>::infinity() ;
        }

    /*----------------------------- calc. aabb for inputs */
        for (auto  _iter  =
             this->_mesh.node().head() ;
                   _iter !=
             this->_mesh.node().tend() ;
                 ++_iter  )
        {
            if (_iter->mark() >= +0)
            {
            this->_bmin[0] = std::min (
            this->_bmin[0] ,
            _iter->pval(0) ) ;
            this->_bmin[1] = std::min (
            this->_bmin[1] ,
            _iter->pval(1) ) ;
            this->_bmin[2] = std::min (
            this->_bmin[2] ,
            _iter->pval(2) ) ;

            this->_bmax[0] = std::max (
            this->_bmax[0] ,
            _iter->pval(0) ) ;
            this->_bmax[1] = std::max (
            this->_bmax[1] ,
            _iter->pval(1) ) ;
            this->_bmax[2] = std::max (
            this->_bmax[2] ,
            _iter->pval(2) ) ;
            }
        }

        float     static const _RTOL =
            std::pow (
            std::numeric_limits<float>
            ::epsilon(), (float) +0.8) ;

        iptr_type static
        constexpr _NBOX=(iptr_type)+8  ;

        float      _BTOL[3] ;
        _BTOL[0] =
       (float)    ( this->_bmax[ 0] -
                    this->_bmin[ 0] )
                 * _RTOL ;
        _BTOL[1] =
       (float)    ( this->_bmax[ 1] -
                    this->_bmin[ 1] )
                 * _RTOL ;
        _BTOL[2] =
       (float)    ( this->_bmax[ 2] -
                    this->_bmin[ 2] )
                 * _RTOL ;

    /*-------------------- make aabb-tree and init. bbox. */
        aabb_mesh( this->_mesh.node(),
                   this->_mesh.tri4(),
                   this->_tree,_BTOL ,
                  _NBOX , tria_pred()) ;
    }

    /*
    --------------------------------------------------------
     * CLIP-HFUN: impose |dh/dx| limits.
    --------------------------------------------------------
     */

    __normal_call void_type clip (
        )
    {
        class  less_than
        {
    /*-------------------- "LESS-THAN" operator for queue */
        public  :
            typename
            real_list::_write_it _hptr;

        public  :
        __inline_call less_than  (
            typename
            real_list::_write_it _hsrc
            ) : _hptr(_hsrc) {}

        __inline_call
            bool_type operator() (
            iptr_type _ipos,
            iptr_type _jpos
            )
        {   return *(this->_hptr+_ipos) <
                   *(this->_hptr+_jpos) ;
        }
        } ;

        typedef typename
            allocator:: size_type   uint_type ;

        uint_type static constexpr
            _null =
        std::numeric_limits<uint_type>::max() ;

        containers::prioritymap <
            iptr_type ,
            less_than ,
            allocator >
        _sort((less_than(this->_hval.head())));

        containers:: array      <
            typename
            allocator:: size_type,
            allocator >     _keys;

        typename
            mesh_type::connector _conn;

    /*-------------------- push nodes onto priority queue */
        _keys.set_count (
            _mesh.node().count() ,
        containers::tight_alloc, _null) ;

        iptr_type _inum  = +0;
        for (auto _iter  =
             this->_mesh.node().head();
                  _iter !=
             this->_mesh.node().tend();
                ++_iter , ++_inum)
        {
            if (_iter->mark() >= +0 )
            {
                _keys[_inum] =
                    _sort.push(_inum) ;
            }
        }

    /*-------------------- compute h(x) via fast-marching */
        for ( ; !_sort.empty() ; )
        {
            iptr_type _base ;
            _sort._pop_root(_base) ;

            _keys[_base] = _null ;

            _conn.set_count( +0) ;
             this->_mesh.
            connect_3(_base, POINT_tag, _conn) ;

            real_type _hnow  = _hval[_base];

            for (auto _next  = _conn.head();
                      _next != _conn.tend();
                    ++_next  )
            {
                if (_next->_kind == TRIA4_tag)
                {
    /*--------------------------------------- TRIA-4 case */
                 auto _cell =_next->_cell;

                 auto _inod = this->
                _mesh. tri4( _cell).node(0);
                 auto _jnod = this->
                _mesh. tri4( _cell).node(1);
                 auto _knod = this->
                _mesh. tri4( _cell).node(2);
                 auto _lnod = this->
                _mesh. tri4( _cell).node(3);

    /*-------------------- skip any cells with null nodes */
                if (_keys[_inod] == _null &&
                    _inod != _base) continue ;
                if (_keys[_jnod] == _null &&
                    _jnod != _base) continue ;
                if (_keys[_knod] == _null &&
                    _knod != _base) continue ;
                if (_keys[_lnod] == _null &&
                    _lnod != _base) continue ;

    /*-------------------- skip cells due to sorted order */
                real_type _hmax;
                _hmax = this->_hval[_inod] ;
                _hmax = std::max(
                _hmax , this->_hval[_jnod]);
                _hmax = std::max(
                _hmax , this->_hval[_knod]);
                _hmax = std::max(
                _hmax , this->_hval[_lnod]);

                if (_hmax <= _hnow) continue ;

    /*-------------------- solve for local |dh/dx| limits */
                real_type _iold =
                     this->_hval[_inod] ;
                real_type _jold =
                     this->_hval[_jnod] ;
                real_type _kold =
                     this->_hval[_knod] ;
                real_type _lold =
                     this->_hval[_lnod] ;

                if (this->_dhdx.count() >1)
                {
    /*-------------------- update adj. set, g = g(x) case */
                if (eikonal_tria_3d (
                   &this->
                _mesh. node( _inod).pval(0),
                   &this->
                _mesh. node( _jnod).pval(0),
                   &this->
                _mesh. node( _knod).pval(0),
                   &this->
                _mesh. node( _lnod).pval(0),
                    this->_hval[_inod],
                    this->_hval[_jnod],
                    this->_hval[_knod],
                    this->_hval[_lnod],
                    this->_dhdx[_inod],
                    this->_dhdx[_jnod],
                    this->_dhdx[_knod],
                    this->_dhdx[_lnod]) )
                {

                if (_keys[_inod] != _null)
                if (_hval[_inod] != _iold)
                    _sort.update(
                    _keys[_inod] ,  _inod) ;

                if (_keys[_jnod] != _null)
                if (_hval[_jnod] != _jold)
                    _sort.update(
                    _keys[_jnod] ,  _jnod) ;

                if (_keys[_knod] != _null)
                if (_hval[_knod] != _kold)
                    _sort.update(
                    _keys[_knod] ,  _knod) ;

                if (_keys[_lnod] != _null)
                if (_hval[_lnod] != _lold)
                    _sort.update(
                    _keys[_lnod] ,  _lnod) ;

                }
                }
                else
                if (this->_dhdx.count()==1)
                {
    /*-------------------- update adj. set, const. g case */
                if (eikonal_tria_3d (
                   &this->
                _mesh. node( _inod).pval(0),
                   &this->
                _mesh. node( _jnod).pval(0),
                   &this->
                _mesh. node( _knod).pval(0),
                   &this->
                _mesh. node( _lnod).pval(0),
                    this->_hval[_inod],
                    this->_hval[_jnod],
                    this->_hval[_knod],
                    this->_hval[_lnod],
                    this->_dhdx[  +0 ],
                    this->_dhdx[  +0 ],
                    this->_dhdx[  +0 ],
                    this->_dhdx[  +0 ]) )
                {

                if (_keys[_inod] != _null)
                if (_hval[_inod] != _iold)
                    _sort.update(
                    _keys[_inod] ,  _inod) ;

                if (_keys[_jnod] != _null)
                if (_hval[_jnod] != _jold)
                    _sort.update(
                    _keys[_jnod] ,  _jnod) ;

                if (_keys[_knod] != _null)
                if (_hval[_knod] != _kold)
                    _sort.update(
                    _keys[_knod] ,  _knod) ;

                if (_keys[_lnod] != _null)
                if (_hval[_lnod] != _lold)
                    _sort.update(
                    _keys[_lnod] ,  _lnod) ;

                }
                }

                }
                else
                if (_next->_kind == HEXA8_tag)
                {
    /*--------------------------------------- HEXA-8 case */



                }
            }
        }

    }

    /*
    --------------------------------------------------------
     * FIND-TRIA: scan for nearest tria.
    --------------------------------------------------------
     */

    class find_tria
        {
    /*--------------------------- point-"in"-tria functor */
        public  :
        real_type              *_ppos ;

        mesh_type              *_mesh ;

        bool_type               _find ;
        iptr_type               _tpos ;

        public  :

    /*------------------------ make a tree-tria predicate */
        __inline_call find_tria (
            real_type*_psrc = nullptr ,
            mesh_type*_msrc = nullptr
            ) : _ppos(_psrc) ,
                _mesh(_msrc) ,
                _find(false) ,
                _tpos(   -1)   {}

    /*------------------------ call pred. on tree matches */
        __inline_call
            void_type operator () (
                typename
            tree_type::item_data  *_iptr
            )
        {
            if (this->_find) return ;

            for ( ; _iptr != nullptr;
                    _iptr = _iptr->_next)
            {
                real_type  _qtmp[+3];
                iptr_type  _TPOS =
                    _iptr->_data.ipos() ;

                if (near_pred ( _ppos ,
                        _qtmp ,*_mesh ,
                        _TPOS ) )
                {
    /*------------------------ is fully inside: finished! */
                this->_find =  true ;
                this->_tpos = _TPOS ;

                break ;
                }
            }
        }

        } ;

    class near_tria
        {
    /*--------------------------- point-near-tria functor */
        public  :
        real_type              *_ppos ;
        real_type              *_qpos ;

        real_type               _dsqr ;

        mesh_type              *_mesh ;

        bool_type               _find ;
        iptr_type               _tpos ;

        public  :

    /*------------------------ make a tree-tria predicate */
        __inline_call near_tria (
            real_type*_psrc = nullptr ,
            real_type*_qsrc = nullptr ,
            mesh_type*_msrc = nullptr
            ) : _ppos(_psrc) ,
                _qpos(_qsrc) ,
           _dsqr(+std::numeric_limits
           <real_type>::infinity()) ,
                _mesh(_msrc) ,
                _find(false) ,
                _tpos(   -1)   {}

    /*------------------------ call pred. on tree matches */
        __inline_call float operator () (
                typename
            tree_type::item_data  *_iptr
            )
        {
            if (this->_find) return +0. ;

            for ( ; _iptr != nullptr;
                    _iptr = _iptr->_next)
            {
                real_type  _qtmp[+3];
                iptr_type  _TPOS =
                    _iptr->_data.ipos() ;

                if (near_pred ( _ppos ,
                        _qtmp ,*_mesh ,
                        _TPOS ) )
                {
    /*------------------------ is fully inside: finished! */
                _qpos[0] = _qtmp[0] ;
                _qpos[1] = _qtmp[1] ;
                _qpos[2] = _qtmp[2] ;

                this->_dsqr =
                    (real_type) +0. ;

                this->_find =  true ;
                this->_tpos = _TPOS ;

                break ;

                }
                else
                {
    /*------------------------ projected match: keep best */
                real_type _dtmp =
            geometry::lensqr_3d(_ppos, _qtmp);

                if (_dtmp<_dsqr )
                {
                _qpos[0] = _qtmp[0] ;
                _qpos[1] = _qtmp[1] ;
                _qpos[2] = _qtmp[2] ;

                this->_dsqr = _dtmp ;
                this->_tpos = _TPOS ;
                }

                }
            }

            return (float) this->_dsqr ;
        }

        } ;

    /*
    --------------------------------------------------------
     * NEAR-PRED: TRUE if PPOS is in TPOS.
    --------------------------------------------------------
     */

    __static_call
    __normal_call bool_type near_pred (
        real_type*_ppos ,
        real_type*_qpos ,
        mesh_type&_mesh ,
        iptr_type _tpos
        )
    {
        geometry::hits_type _hits ;
        if (geometry::proj_tria_3d(_ppos,
           &_mesh. node(
            _mesh. tri4(
            _tpos).node(0)).pval(0) ,
           &_mesh. node(
            _mesh. tri4(
            _tpos).node(1)).pval(0) ,
           &_mesh. node(
            _mesh. tri4(
            _tpos).node(2)).pval(0) ,
           &_mesh. node(
            _mesh. tri4(
            _tpos).node(3)).pval(0) ,
            _qpos,_hits) )
        {
            return (_hits ==
                 geometry::tria_hits) ;
        }
        else
        {
            return ( false ) ;
        }
    }

    /*
    --------------------------------------------------------
     * HINT: check for valid index.
    --------------------------------------------------------
     */

    __inline_call bool_type hint_okay (
        hint_type _hint
        )
    {
    /*------------------------- test whether hint is valid */
        iptr_type _iplo = (iptr_type)+0 ;

        iptr_type _iphi = (iptr_type)
            this->_mesh.tri4 ().count() ;

        return _hint >= _iplo &&
               _hint <  _iphi &&
        this-> _mesh.tri4(_hint).mark() >= +0 ;
    }

    /*
    --------------------------------------------------------
     * EVAL: eval. size-fun. value.
    --------------------------------------------------------
     */

    __normal_call real_type eval (
        real_type *_ppos ,
        hint_type &_hint
        )
    /*------------------------ find tria + linear interp. */
    {
        real_type  _QPOS[3] = {
       (real_type) _ppos[0] ,
       (real_type) _ppos[1] ,
       (real_type) _ppos[2] } ;

        float      _PPOS[3] = {
       (float)     _ppos[0] ,
       (float)     _ppos[1] ,
       (float)     _ppos[2] } ;

        real_type _hOUT =
    +std::numeric_limits<real_type>::infinity() ;

        if (hint_okay(_hint))
        {
    /*------------------------ test whether hint is valid */
            if(!near_pred( _ppos,
                    _QPOS, _mesh,
                    _hint)  )
            {
            _hint =  this->null_hint();
            }
        }
        else
        {
    /*------------------------ hint is definitely invalid */
            _hint =  this->null_hint();
        }

        if (_hint == this->null_hint())
        {
    /*------------------------ scan to find bounding tria */
            tree_pred _pred (_PPOS) ;
            find_tria _func (_ppos,
                            &_mesh) ;

            this->
           _tree.find(_pred, _func) ;

           _hint  = _func._find ?
                    _func._tpos :
            hfun_type::null_hint () ;
        }

        if (_hint == this->null_hint())
        {
    /*------------------------ outside: find nearest tria */
            near_tria _func (_ppos,
                      _QPOS,&_mesh) ;

            this->
           _tree.near(_PPOS, _func) ;

           _hint =    _func. _tpos;
        }

        if (_hint != this->null_hint())
        {
    /*------------------------ linear interp. within tria */
        real_type _hsum = (real_type)+.0 ;
        real_type _vsum = (real_type)+.0 ;

        for(auto _fpos = 4; _fpos-- != 0; )
        {
            iptr_type  _fnod [4] ;
            tri4_type::
            face_node(_fnod, _fpos, 3, 2) ;

            _fnod[0] = this->_mesh.
             tri4(_hint).node(_fnod[0]);
            _fnod[1] = this->_mesh.
             tri4(_hint).node(_fnod[1]);
            _fnod[2] = this->_mesh.
             tri4(_hint).node(_fnod[2]);

            _fnod[3] = this->_mesh.
             tri4(_hint).node(_fnod[3]);

            real_type _tvol =
                geometry::tetra_vol_3d (
               &this->_mesh.
                node(_fnod[0]).pval(0) ,
               &this->_mesh.
                node(_fnod[1]).pval(0) ,
               &this->_mesh.
                node(_fnod[2]).pval(0) ,
               _QPOS) ;

            _hsum += _tvol *
                this->_hval[_fnod [ 3]] ;

            _vsum += _tvol ;
        }


        _hOUT = _hsum / _vsum ;

        }

    /*------------------------- size-fun interp. to ppos. */
        return  _hOUT ;
    }

    } ;


    }

#   endif   //__HFUN_MESH_EUCLIDEAN_3__



