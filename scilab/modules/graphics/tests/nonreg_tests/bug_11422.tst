// =============================================================================
// Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
// Copyright (C) 2012 - Scilab Enterprises - Vincent COUVERT
//
//  This file is distributed under the same license as the Scilab package.
// =============================================================================

// <-- TEST WITH GRAPHIC -->

// <-- Non-regression test for bug 11422 -->
//
// <-- Bugzilla URL -->
// http://bugzilla.scilab.org/show_bug.cgi?id=11422
//
// <-- Short Description -->
// datatips do not work when the figure contains uicontrols.

uicontrol()
assert_checktrue(execstr("datatipEventhandler(get(gcf(), ""figure_id""), 1 ,1, 1)", "errcatch")==0);