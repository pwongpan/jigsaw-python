
    /*
    --------------------------------------------------------
     * RVOR-UPDATE-2: update restricted voronoi in R^2.
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
     * Last updated: 09 April, 2018
     *
     * Copyright 2013-2018
     * Darren Engwirda
     * de2363@columbia.edu
     * https://github.com/dengwirda/
     *
    --------------------------------------------------------
     */

    // from rvor_mesh_2.hpp


    /*
    --------------------------------------------------------
     * _POP-EDGE: delete edge from restricted-tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type _pop_edge (
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
        for(auto _fpos =+3; _fpos-- != +0; )
        {
    /*--------------------------------- get edge indexing */
            iptr_type _tnod[ +3] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +2, +1);
            _tnod[0] = _mesh._tria.
                tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
                tria(_tpos)->node(_tnod[1]);

            algorithms::isort (
                &_tnod[0], &_tnod[2],
                    std::less<iptr_type>());

            edge_data _edat, _same ;
            _edat._node[0] = _tnod[0];
            _edat._node[1] = _tnod[1];

    /*--------------------------------- remove if present */
            _mesh._pop_edge(_edat, _same)  ;
        }
    }

    /*
    --------------------------------------------------------
     * INIT-BALL: add new ball to restricted-tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type init_ball (
        mesh_type &_mesh,
        geom_type &_geom,
        hfun_type &_hfun,
        iptr_type  _npos,
        ball_list &_bset,
        ball_list &_bscr,
        char_type  _kind,
        iptr_type  _pass,
        rdel_opts &_opts
        )
    {
        __unreferenced (_geom) ;
        __unreferenced (_opts) ;

        if (_kind == feat_ball)
        {
        if (_mesh._tria.
             node(_npos)->feat()==hard_feat)
        {
    /*---------- push protecting ball for "hard" features */
            ball_data _ball ;
            _ball._node[0] = _npos;

            _ball._pass    = _pass;
            _ball._kind    = _kind;

            _ball._ball[0] = _mesh.
                _tria.node(_npos)->pval(0) ;
            _ball._ball[1] = _mesh.
                _tria.node(_npos)->pval(1) ;

            real_type _rbal = _hfun.eval (
               &_mesh._tria.
            node(_npos)->pval(0) ,
                _mesh._tria.
            node(_npos)->idxh()) ;

            _ball._ball[2] = _rbal * _rbal ;

            _bset.push_tail (_ball) ;
            _bscr.push_tail (_ball) ;
        }
        }
        else
        if (_kind == voro_ball)
        {
    /*---------- push protecting ball for voronoi "cover" */
            ball_data _ball ;
            _ball._node[0] = _npos;

            _ball._pass    = _pass;
            _ball._kind    = _kind;

            _ball._ball[0] = _mesh.
                _tria.node(_npos)->pval(0) ;
            _ball._ball[1] = _mesh.
                _tria.node(_npos)->pval(1) ;

            real_type _rbal = _hfun.eval (
               &_mesh._tria.
            node(_npos)->pval(0) ,
                _mesh._tria.
            node(_npos)->idxh()) ;

            _rbal *= (real_type).67 ;

            _ball._ball[2] = _rbal * _rbal ;

            _bset.push_tail (_ball) ;
            _bscr.push_tail (_ball) ;
        }
    }

    /*
    --------------------------------------------------------
     * PUSH-BALL: add new ball to restricted-tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_ball (
        mesh_type &_mesh ,
        geom_type &_geom ,
        hfun_type &_hfun ,
        iptr_type  _tpos ,
        ball_list &_bset ,
        ball_list &_bscr ,
        typename
    mesh_type::ball_list & _ball_test ,
        iptr_type &_nbal ,
        char_type  _kind ,
        iptr_type  _pass ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        for(char_type _npos =+3; _npos-- != +0; )
        {
            iptr_type _node =
                _mesh._tria.
            tria( _tpos)->node(_npos) ;

            ball_data _bdat ;
            _bdat._node[0] = _node;
            _bdat._kind    = _kind;
            _bdat._pass    = _pass;

            typename mesh_type::
                     node_list::
                item_type *_mptr = nullptr;
            if(_ball_test.
                find (_bdat, _mptr) )
            {
        /*--------------------------- don't test repeats! */
                continue   ;
            }

            _ball_test.push (_bdat) ;

        /*--------------------------- call ball predicate */
            mesh_pred::ball_cost (
                _geom,_hfun,
                _mesh,
                _node,_kind,


                _flag)   ;

        /*--------------------------- push edge onto mesh */
            if (_flag
                    == mesh::ring_item)
                _bscr. push_tail(_bdat) ;

            if (_flag
                    != mesh::null_item)
                _nbal += +1 ;

        } // for(iptr_type _npos = +3 ; _npos-- != +0 ; )
    }

    /*
    --------------------------------------------------------
     * PUSH-EDGE: add new edge to restricted-tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_edge (
        mesh_type &_mesh ,
        geom_type &_geom ,
        hfun_type &_hfun ,
        iptr_type  _tpos ,
        edat_list &_eset ,
        escr_list &_escr ,
        typename
    mesh_type::edge_list & _edge_test ,
        iptr_type &_nedg ,
        iptr_type &_ndup ,
        iptr_type  _pass ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- correct node dims? */
        iptr_type _fdim =+0 ;
        for(iptr_type _node =+3; _node-- != +0; )
        {
        if (_mesh._tria.node (
            _mesh._tria.tria (
            _tpos)->node(_node))->fdim() <= +1)
            _fdim += +1 ;
        }
    /*-------------------------------- quick break if not */
        if (_fdim  < +2 ) return ;

    /*-------------------------------- check "restricted" */
        for(char_type _fpos =+3; _fpos-- != +0; )
        {
        /*---------------------------- extract face nodes */
            iptr_type _tnod[ +3] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +2, +1) ;
            _tnod[0] = _mesh._tria.
            tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
            tria(_tpos)->node(_tnod[1]);

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tnod[0])->fdim() > 1 ||
                _mesh._tria.node(
                _tnod[1])->fdim() > 1 )
                continue   ;

            algorithms::isort (
                &_tnod[0], &_tnod[2],
                    std::less<iptr_type>()) ;

            edge_data _fdat;
            _fdat._node[0] = _tnod[0] ;
            _fdat._node[1] = _tnod[1] ;

            edge_cost _cdat;
            _cdat._node[0] = _tnod[0] ;
            _cdat._node[1] = _tnod[1] ;

            typename mesh_type::
                     edge_list::
                item_type *_mptr = nullptr;
            if(_edge_test.
                find( _fdat, _mptr) )
            {
        /*--------------------------- count bnd. repeats! */
                _ndup +=
                _mptr->_data._dups;

        /*--------------------------- don't test repeats! */
                continue   ;
            }

            _cdat._pass    = _pass;
            _fdat._pass    = _pass;

            _fdat._tadj    = _tpos;
            _fdat._eadj    = _fpos;
            _fdat._dups    = +0; // count num. dup's
                                 // only in hash-set

        /*--------------------------- call face predicate */
            char_type _feat, _topo;
            real_type _fbal[ 3];
            real_type _sbal[ 3];
            mesh_pred::edge_cost (
                _geom,_hfun,
                _mesh,
                _fdat._tadj,
                _fdat._eadj,
                _opts,_cdat,
                _fdat._part,
                _feat,_topo,
                _fdat._kind,
                _fbal,_sbal)   ;

        /*--------------------------- push edge onto mesh */
            if (_fdat._kind
                    == mesh::ring_item)
                _escr. push_tail(_cdat) ;

            if (_fdat._kind
                    != mesh::null_item)
                _nedg += +1 ;

            if (_fdat._kind
                    != mesh::null_item)
                _fdat.
                _dups  = +1 ;

            if (_fdat._kind
                    != mesh::null_item)
                _eset. push_tail(_fdat) ;


            _edge_test.push (_fdat) ;

        } // for(iptr_type _fpos = +3 ; _fpos-- != +0 ; )
    }

    /*
    --------------------------------------------------------
     * PUSH-TRIA: add new tria to restricted-tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_tria (
        mesh_type &_mesh ,
        geom_type &_geom ,
        hfun_type &_hfun ,
        iptr_type  _tpos ,
        iptr_type &_sign ,
        tdat_list &_tset ,
        tscr_list &_tscr ,
        typename
    mesh_type::tria_list & _tria_test ,
        iptr_type &_ntri ,
        iptr_type  _pass ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        {
            iptr_type  _tnod[ +3] ;
            _tnod[0] = _mesh.
            _tria.tria(_tpos)->node(0);
            _tnod[1] = _mesh.
            _tria.tria(_tpos)->node(1);
            _tnod[2] = _mesh.
            _tria.tria(_tpos)->node(2);

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tnod[0])->fdim() > 2 ||
                _mesh._tria.node(
                _tnod[1])->fdim() > 2 ||
                _mesh._tria.node(
                _tnod[2])->fdim() > 2 )
                return ;

            tria_data _tdat;
            _tdat._node[0] = _tnod[0] ;
            _tdat._node[1] = _tnod[1] ;
            _tdat._node[2] = _tnod[2] ;

            _tdat._tadj    = _tpos;

            tria_cost _cdat;
            _cdat._node[0] = _tnod[0] ;
            _cdat._node[1] = _tnod[1] ;
            _cdat._node[2] = _tnod[2] ;

            typename mesh_type::
                     tria_list::
                     item_type *_mptr=nullptr;
            if(_tria_test.
                find( _tdat, _mptr) )
            {
        /*--------------------------- don't test repeats! */
                return ;
            }

        //!!_tria_test.push( _tdat) ; won't have repeats!

        /*--------------------------- call tria predicate */
            _tdat._part =  _sign ;

            _cdat._pass =  _pass ;
            _tdat._pass =  _pass ;

            mesh_pred::tria_cost (
                _geom,_hfun,
                _mesh,
                _tdat._tadj,
                _opts,_cdat,
                _tdat._part,
                _tdat._kind)   ;

            _sign = _tdat. _part ;

        /*--------------------------- push edge onto mesh */
            if (_tdat._kind
                    == mesh::ring_item)
                _tscr. push_tail(_cdat) ;

            if (_tdat._kind
                    != mesh::null_item)
                _ntri += +1 ;

            if (_tdat._kind
                    != mesh::null_item)
                _tset. push_tail(_tdat) ;
        }
    }

    /*
    --------------------------------------------------------
     * TRIA-CIRC: calc. circumball for tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type tria_circ ( // get "exact" tbal?
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
        iptr_type _tnod[3] = {
        _mesh.
        _tria.tria( _tpos)->node(0) ,
        _mesh.
        _tria.tria( _tpos)->node(1) ,
        _mesh.
        _tria.tria( _tpos)->node(2)
            } ;

        algorithms::isort(
            &_tnod[0], &_tnod[3],
                std::less<iptr_type>()) ;

    /*---------------------- calc. ball in floating-point */
        real_type _tbal[3] ;
        geometry::circ_ball_2d (
            _tbal ,
       &_mesh._tria.
            node(_tnod[0])->pval(0) ,
       &_mesh._tria.
            node(_tnod[1])->pval(0) ,
       &_mesh._tria.
            node(_tnod[2])->pval(0)
            ) ;

        _mesh._tria.tria(
        _tpos)->circ(0) =  _tbal[0] ;
        _mesh._tria.tria(
        _tpos)->circ(1) =  _tbal[1] ;
    }

    /*
    --------------------------------------------------------
     * PUSH-RDEL: push faces onto restricted tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_rdel (
        geom_type &_geom ,
        hfun_type &_hfun ,
        mesh_type &_mesh ,
        iptr_list &_nnew ,
        iptr_list &_tnew ,
        escr_list &_escr ,
        edat_list &_edat ,
        ball_list &_bscr ,
        ball_list &_bdat ,
        iptr_type  _sign ,
        iptr_type  _pass ,
        mode_type  _dim0 ,
        mode_type  _dim1 ,
        rdel_opts &_opts
        )
    {
        __unreferenced( _sign) ;

    /*------------------------- init. for local hash obj. */
        typename
            mesh_type::node_list _bset (
        typename mesh_type::ball_hash(),
        typename mesh_type::ball_pred(),
           +.8, _mesh._bset.get_alloc()) ;
        typename
            mesh_type::edge_list _eset (
        typename mesh_type::edge_hash(),
        typename mesh_type::edge_pred(),
           +.8, _mesh._eset.get_alloc()) ;

    /*------------------------- push alloc. for hash obj. */
        _bset._lptr.set_count (
            _tnew.count() * +3 ,
        containers::loose_alloc, nullptr);
        _eset._lptr.set_count (
            _tnew.count() * +3 ,
        containers::loose_alloc, nullptr);

    /*------------------------- no. "restricted" subfaces */
        iptr_type _nedg = +0 ;
        iptr_type _nbal = +0 ;
        iptr_type _ntri = +0 ;

        iptr_type _ndup = +0 ;

    /*------------------------- calc. voronoi-dual points */
        for( auto _iter  = _tnew.head();
                  _iter != _tnew.tend();
                ++_iter  )
        {
            tria_circ(_mesh,*_iter) ;
        }

    /*------------- push any new protecting balls created */
        if (_dim0 <= feat_mode &&
            _dim1 >= feat_mode )
        {
        for( auto _iter  = _nnew.head();
                  _iter != _nnew.tend();
                ++_iter  )
        {
            char_type _kind = feat_ball;

            init_ball(_mesh, _geom ,
                      _hfun,*_iter ,
                      _bdat, _bscr ,
                      _kind,
                      _pass, _opts) ;
        }
        }
    /*------------- push any new restricted edges created */
        if (_dim0 <= edge_mode &&
            _dim1 >= edge_mode )
        {
        for( auto _iter  = _tnew.head();
                  _iter != _tnew.tend();
                ++_iter  )
        {
            push_edge(_mesh, _geom ,
                      _hfun,*_iter ,
                      _edat, _escr ,
                      _eset, _nedg ,
                      _ndup,
                      _pass, _opts) ;
        }
        }
    /*------------- push any new restricted balls created */
        if (_dim0 <= ball_mode &&
            _dim1 >= ball_mode )
        {
        for( auto _iter  = _nnew.head();
                  _iter != _nnew.tend();
                ++_iter  )
        {
            char_type _kind = voro_ball;

            init_ball(_mesh, _geom ,
                      _hfun,*_iter ,
                      _bdat, _bscr ,
                      _kind,
                      _pass, _opts) ;
        }
        for( auto _iter  = _tnew.head();
                  _iter != _tnew.tend();
                ++_iter  )
        {
            push_ball(_mesh, _geom ,
                      _hfun,*_iter ,
                      _bdat, _bscr ,
                      _bset,
                      _pass, _opts) ;
        }
        }
    }

    /*
    --------------------------------------------------------
     * PUSH-RVOR: push faces onto restricted dual.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type push_rvor (
        geom_type &_geom ,
        hfun_type &_hfun ,
        mesh_type &_mesh ,
        iptr_list &_nnew ,
        iptr_list &_tnew ,
        iptr_type  _sign ,
        iptr_type  _pass ,
        mode_type  _dim0 ,
        mode_type  _dim1 ,
        rdel_opts &_opts
        )
    {
        __unreferenced( _sign) ;

    /*------------------------- init. for local hash obj. */
        typename
            mesh_type::tria_list _tset (
        typename mesh_type::tria_hash(),
        typename mesh_type::tria_pred(),
           +.8, _mesh._tset.get_alloc()) ;

    /*------------------------- push alloc. for hash obj. */
        _tset._lptr.set_count (
            _tnew.count() * +1 ,
        containers::loose_alloc, nullptr);

    /*------------------------- no. "restricted" subfaces */
        iptr_type _ntri  =  +0 ;

    /*------------------------- calc. voronoi-dual points */
        for( auto _iter  = _tnew.head();
                  _iter != _tnew.tend();
                ++_iter  )
        {
            tria_circ(_mesh,*_iter) ;
        }

    /*------------- push any new restricted edges created */
        if (_dim0 <= tria_mode &&
            _dim1 >= tria_mode )
        {
        for( auto _iter  = _tnew.head();
                  _iter != _tnew.tend();
                ++_iter  )
        {
            push_tria(_mesh, _geom ,
                      _hfun,*_iter ,
                      _tdat, _tscr ,
                      _tset, _ntri ,
                      _pass, _opts) ;
        }
        }
    }



