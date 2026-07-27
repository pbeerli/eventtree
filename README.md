# Event Tree (et)

A small desktop viewer for phylogenetic trees that carry **events** on their
branches — most importantly the migration events estimated by
[migrate-n](https://peterbeerli.com/migrate-html5/), but also confidence
intervals on node ages and ordinary node labels. Trees are read from Newick or
NEXUS files and can be printed or exported as PDF, EPS or SVG.

**Event Tree was written by Michal Palczewski** (Florida State University,
2006–2008). Peter Beerli maintains the code: keeping it compiling on current
compilers and wxWidgets, and fixing what breaks as the file formats produced by
migrate evolve.

## What it draws

A normal Newick tree is drawn as a rectangular cladogram scaled by branch
length. On top of that, comments of the form `[& ... ]` attached to a branch are
interpreted as events:

| Syntax | Meaning | Drawn as |
| --- | --- | --- |
| `[&M from to :time]` | a migration (or, backwards in time, an immigration) from population `from` into population `to`, `time` units below the node | a vertical arrow across the branch, labelled with both population names |
| `[&I lower upper]` | a credibility interval for the age of the node | a horizontal bar spanning the interval |
| `(a,b)0.95:1` | a label on the branch (support value, name, anything) | text next to the branch |

A branch may carry several events; migrate routinely writes two or more per
branch:

```
(A:1.56 [&M 1 2 :0.56] [&M 2 1:0.5],(B:0.5,C:0.5):1.06);
```

Any other `[& ... ]` comment is ignored, including the per-locus headers that
migrate writes into its `treefile`:

```
[& Locus 3, best ln(L) = -105593.947787 (c=coalescent node, t=tipnode) ]
```

so a multi-locus `treefile` can be opened as it comes out of migrate. Blank
lines between trees are fine, and every tree in the file is loaded — use the
back/forward buttons to page through the loci.

## Input files

* **Newick / PHYLIP** — one or many trees in a file, each terminated by `;`.
* **NEXUS** — a `TREES` block, including `TRANSLATE` tables, parsed with the
  NEXUS Class Library (NCL 2.0, Paul O. Lewis) bundled in `src/ncl`.

The file type is detected from the contents, not from the extension. Example
files live in `data/`.

## Using it

Open files from the *File* menu, drop them on the window, or name them on the
command line:

```
et data/migtreefile data/testtrees
```

Each file gets its own tab; the toolbar arrows move between the trees inside the
current file. *File → Export* writes the tree currently on screen as PDF, EPS or
SVG (WMF on Windows); *File → Print* and *Print Preview* use the platform
printer.

## Building

Requirements: a C/C++ compiler, GNU make, and **wxWidgets 3.x** with `wx-config`
on the `PATH`. Everything else — the Haru free PDF library (Takeshi Kanno) and
NCL — is bundled.

```sh
./configure
make
```

The binary is `src/et`; `make install` puts it in `$prefix/bin`. Always run
`configure` before `make` — the checked-in `Makefile`s carry whatever wxWidgets
version and architecture they were last configured for.

Two things to know if the build fails on a fresh machine:

* The autotools helper files at the top level (`install-sh`, `missing`,
  `depcomp`, `compile`, `COPYING`, `INSTALL`) are symlinks into an automake
  installation. When the package manager updates automake they dangle and
  `configure` stops with *cannot find install-sh*; re-point them at the current
  `share/automake-<version>` directory. Do **not** run `autoreconf` to fix this:
  `configure.in` still uses macros (`AC_INIT(configure.in)`,
  `AM_CONFIG_HEADER`) that current autoconf and automake reject.
* wxWidgets 3.2 dropped the separate `svg` library — `wxSVGFileDC` now lives in
  core — so `configure.in` asks `wx-config` for `core base xml` only.

## Layout

```
src/            the program: tree parsing (treelib.c), file reading
                (TreeFileReader.cpp), drawing (Treeview.cpp) and the
                output device classes (PdfDC, EpsDC, SvgDC, WmfDC)
src/haru/       bundled libharu, for PDF output
src/ncl/        bundled NEXUS Class Library, for NEXUS input
data/           example tree files
```

The tree parser in `src/treelib.c` is plain C and independent of wxWidgets: it
turns a Newick string into a `Tree`, then into a list of drawing primitives
(lines, arrows, text) that each output device renders in its own way.

## History

Development happened at
[bitbucket.org/mpalczewski/et](https://bitbucket.org/mpalczewski/et), which had
been dormant for years; this repository continues from that history.

## License

GPL version 2 or later. Copyright 2006–2008 Michal Palczewski and Peter Beerli,
Florida State University. See `COPYING`.
