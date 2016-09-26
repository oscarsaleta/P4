
// definitions

#define DIM     100
#define DIM1    (DIM+1)
#define DIM2    (2*DIM)
#define PI       3.1415926535897932384626433832

// structures

struct poly
{
	int degz, degzb;
	double re, im;
	struct poly * next_poly;
};

struct hom_poly
{
	int total_degree;
	struct poly * p;
	struct hom_poly * next_hom_poly;
};

// variables

extern bool env_maple;
extern bool env_reduce;
extern bool env_windows;
extern int weakness_level;
extern double precision;
extern char * win_sumtablepath;
extern struct hom_poly * vec_field;

// prototypes

extern void create_sum( int );
extern void check_sum( char *, int );
extern void read_table( const char * );
extern void ins_hom_poly( struct hom_poly *, int, int, double, double );
extern void ins_poly( struct poly *, int, int, double, double );
extern struct poly * copy_poly( struct poly * );
extern struct poly * search_poly( struct poly *, int, int, int * );
extern void ins_coeff( struct poly *, int, int, double, double );
extern void delete_coeff( struct poly * );
extern void add_poly( struct poly *, struct poly * );
extern void sub_poly( struct poly *, struct poly * );
extern void term_prod( struct poly *, int, int, double, double );
extern void prod_poly( struct poly *, struct poly * );
extern void multc_poly( struct poly *, double, double );
extern void delete_poly(struct poly ** );
extern struct poly * conj_poly( struct poly * );
extern void print_poly( struct poly * );
extern void read_poly( struct poly * );
extern struct poly * find_poly( struct hom_poly *, int );
extern void diff( struct poly * );
extern void G( struct poly * );
extern void Imgz( struct poly *, struct poly * );
extern void Regz( struct poly *, struct poly * );
extern void LL( struct poly *, struct poly *, int, double * );
extern double part_lyapunov_coeff( char *, int );
