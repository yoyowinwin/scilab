/*
 * Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 * Copyright (C) 2012 - Marcos CARDINOT
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */
package org.scilab.modules.gui.ged;

import java.awt.CardLayout;
import java.awt.Dimension;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;

import org.scilab.modules.gui.ged.actions.CloseAction;
import org.scilab.modules.gui.ged.actions.ShowHide;
import org.scilab.modules.gui.bridge.tab.SwingScilabTab;
import org.scilab.modules.gui.menu.Menu;
import org.scilab.modules.gui.menu.ScilabMenu;
import org.scilab.modules.gui.menubar.MenuBar;
import org.scilab.modules.gui.menubar.ScilabMenuBar;
import org.scilab.modules.gui.textbox.TextBox;
import org.scilab.modules.gui.toolbar.ScilabToolBar;
import org.scilab.modules.gui.toolbar.ToolBar;
import org.scilab.modules.gui.utils.WindowsConfigurationManager;

/**
 * Swing implementation of Scilab Graphic Editor.
 *
 * @author Marcos CARDINOT <mcardinot@gmail.com>
 */
public class SwingInspector extends SwingScilabTab {
    private JScrollPane desktop;
    public static JPanel pReceive;
    private MenuBar menuBar;
    private Menu fileMenu;
    public static final String INSPECTORUUID = "4m249547-6a71-4998-r8c-00o367s47932";

    private static String imagepath = System.getenv("SCI") + "/modules/gui/images/icons/";
    public static String icon_color_fill = imagepath + "16x16/actions/color-fill.png";
    public static String icon_collapse = imagepath + "16x16/actions/tree-collapse.png";
    public static String icon_expand = imagepath + "16x16/actions/media-playback-start.png";
    public static String icon_expand_all = imagepath + "32x32/actions/tree-diagramm.png";
    public static String icon_collapse_all = imagepath + "32x32/actions/tree-diagramm-delete.png";

    /**
    * Constructor
    *
    * @param selected Indicates which property window will open.
    * @param objectID Enters the identification of object.
    */
    public SwingInspector(SelectionEnum selected, String objectID, Integer clickX, Integer clickY) {
        super(MessagesGED.quick_ged, INSPECTORUUID);

        setAssociatedXMLIDForHelp("quickged");

        buildMenuBar();
        addMenuBar(menuBar);

        ToolBar toolBar = ScilabToolBar.createToolBar();
        toolBar.add(ShowHide.createButton(MessagesGED.hide));
        toolBar.addSeparator();

        guiComponents();
        new SwapObject(selected, objectID, clickX, clickY);

        setContentPane(desktop);
        WindowsConfigurationManager.restorationFinished(this);
        addToolBar(toolBar);
    }

    /**
    * It has all the components of the inspector panel.
    */
    private void guiComponents() {
        desktop = new JScrollPane();
        pReceive = new JPanel();

        desktop.setBorder(null);
        desktop.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        desktop.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        desktop.setAlignmentX(0.0F);
        desktop.setAlignmentY(0.0F);

        pReceive.setAlignmentX(0.0F);
        pReceive.setAlignmentY(0.0F);
        pReceive.setLayout(new CardLayout());

        desktop.setViewportView(pReceive);
    }

    /**
     * Creates the menuBar
     */
    public void buildMenuBar() {
        menuBar = ScilabMenuBar.createMenuBar();

        fileMenu = ScilabMenu.createMenu();
        fileMenu.setText(MessagesGED.file);
        fileMenu.setMnemonic('F');
        fileMenu.add(CloseAction.createMenu());

        menuBar.add(fileMenu);
    }

    /**
     * {@inheritDoc}
     */
    public void addInfoBar(TextBox infoBarToAdd) {
        setInfoBar(infoBarToAdd);
    }

    /**
     * {@inheritDoc}
     */
    public void addMenuBar(MenuBar menuBarToAdd) {
        setMenuBar(menuBarToAdd);
    }

    /**
     * {@inheritDoc}
     */
    public void addToolBar(ToolBar toolBarToAdd) {
        setToolBar(toolBarToAdd);
    }
}