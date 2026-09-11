c Software for Large-scale Unconstrained Optimization
c
c L-BFGS is a limited-memory quasi-Newton code for unconstrained 
c    optimization. The code has been developed at the 
c    Optimization Technology Center,
c    a joint venture of Argonne National Laboratory and Northwestern University.
c
c Downloading and Installing L-BFGS
c    http://www.ece.nwu.edu/~nocedal/Software/lbfgs_um.tar.gz
c
c You are welcome to grab the full Unix distribution, 
c     containing source code, makefile, and user guide. 
c
c      Condition for Use: This software is freely available for 
c      non-commercial uses. We expect that all publications describing 
c      work using this software
c      quote at least one of the references given below. 
c
c
c References
c
c      J. Nocedal. Updating Quasi-Newton Matrices with Limited Storage (1980), 
c                  Mathematics of Computation 35, pp. 773-782. 
c      D.C. Liu and J. Nocedal. On the Limited Memory Method for 
c           Large Scale Optimization (1989), Mathematical Programming B, 45, 3, 
c           pp. 503-528. 
c
c  For commercial use of the software contact Jorge Nocedal
c   L-BFGS-B (version 2.1)
c 
c  $Id: $
c 
c================ 
c     
      subroutine LBFGSBWRAPPER(n, m, x, f, g, l, u, nbd, 
     +                         wa, iwa, task,
     +                         iprint, csave, lsave, isave, dsave)
 
      integer          n, m, iwa(3*n), nbd(n),iprint, isave(44)
      double precision f, 
     +                 x(n), l(n), u(n), g(n),
     +                 wa(2*m*n+4*n+12*m*m+12*m),dsave(44)
      character*60    task, csave
      logical          lsave(4)


c================

c     Additional variables
c      character*60     task, csave
c      logical          lsave(4)
c      integer          iprint, isave(44)
c      double precision factr, pgtol, dsave(29)
      double precision factr, pgtol
                    

 
c     We wish to have output at every iteration.

c      iprint = 1

c     We specify the tolerances in the stopping criteria.

      factr=1.0d+2
      pgtol=1.0d-5


c     Output info...
c      write (6,16)
c  16  format(/,5x, 'Solving sample problem.',
c     +       /,5x, ' (f = 0.0 at the optimal solution.)',/)               



c        ------- Ancient beginning of the loop ----------
 
 111  continue
      
c     This is the call to the L-BFGS-B code.
 
      call setulb(n,m,x,l,u,nbd,f,g,factr,pgtol,wa,iwa,task,iprint,
     +            csave,lsave,isave,dsave)

 

c         ---------- Ancient end of the loop -------------
 

 200  return
 
      end
