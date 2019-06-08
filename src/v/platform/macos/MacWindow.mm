//
// Created by 小白白 on 2019-06-06.
//

#include "hlmv.h"
#include <QWindow>
#include <QMacToolBar>
#include <QMacNativeWidget>
#include <QMacCocoaViewContainer>
#include <QPropertyAnimation>
#import <AppKit/AppKit.h>

#include "ViewerSettings.h"

@interface MyWindowDelegate : NSObject<NSWindowDelegate>
- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions;
- (BOOL)windowShouldClose:(id)sender;
@end
@implementation MyWindowDelegate
- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions;
{
	return (NSApplicationPresentationFullScreen |
	        NSApplicationPresentationHideDock |
	        NSApplicationPresentationAutoHideMenuBar |
	        NSApplicationPresentationAutoHideToolbar);
}
- (BOOL)windowShouldClose:(id)sender {
	exit(0);
	return YES;
}
@end

void QtGuiApplication1::SetupMacWindow()
{
	ui.statusBar->setVisible(false);

	NSApplication *nsa = NSApp;

	NSWindow* nsw = [reinterpret_cast<NSView *>(this->windowHandle()->winId()) window];

	MyWindowDelegate *mwd = [[MyWindowDelegate alloc] init];
	nsw.delegate = mwd;

	nsw.styleMask |=  NSWindowStyleMaskUnifiedTitleAndToolbar;
	//nsw.styleMask |=  NSWindowStyleMaskFullSizeContentView;
	//nsw.titleVisibility = NSWindowTitleHidden;
	nsw.titlebarAppearsTransparent = TRUE;

	QMacToolBar *toolBar = new QMacToolBar(this);
	NSToolbar *nst = toolBar->nativeToolbar();
	nst.displayMode = NSToolbarDisplayModeLabelOnly;
	nst.sizeMode = NSToolbarSizeModeRegular;

	// pages
	toolBar->addStandardItem(QMacToolBarItem::FlexibleSpace);
	toolBar->addStandardItem(QMacToolBarItem::Space);
	toolBar->addSeparator();
	QTabWidget *tabWidget = ui.tabWidget;
	tabWidget->setVisible(false);
	tabWidget->tabBar()->setVisible(false);

	for(int i=0;i<tabWidget->count();++i)
	{
		QMacToolBarItem *item = toolBar->addItem(tabWidget->tabIcon(i), tabWidget->tabText(i));
		item->setSelectable(true);
		NSToolbarItem *nativeItem = item->nativeToolBarItem();

		if(tabWidget->currentIndex() == i)
		{
			[nst setSelectedItemIdentifier:[nativeItem itemIdentifier]];
		}

		auto act_lambda = [=]() {
			if(tabWidget->currentIndex() == i) {
				if(!tabWidget->isVisible() )  {
					tabWidget->setVisible(true);
					ui.statusBar->setVisible(true);
				}
				else  {
					tabWidget->setVisible(false);
					ui.statusBar->setVisible(false);
				}

				//[nst setSelectedItemIdentifier:nil];
			}
			else {
				tabWidget->setCurrentWidget(tabWidget->widget(i));
			}
		};

		connect(item,&QMacToolBarItem::activated, act_lambda);
	}
	toolBar->addSeparator();
	toolBar->addStandardItem(QMacToolBarItem::FlexibleSpace);

	if(0)
	{


		QMacToolBarItem *item = toolBar->addItem(this->windowIcon(), QString("About"));
		item->setSelectable(false);
		connect(
				item,
				&QMacToolBarItem::activated,
				[=](){ OnActionAbout(); }
		);

		NSToolbarItem *nativeItem = item->nativeToolBarItem();

		/*
		NSSearchField *search = [[NSSearchField alloc] init];

		[nativeItem setView:search];
		[nativeItem setMaxSize:{30.0, 0.0}];
		 */
	}
	else
	{
		toolBar->addStandardItem(QMacToolBarItem::Space);
	}


	toolBar->attachToWindow(this->window()->windowHandle());


	NSButton *closeBtn = [nsw standardWindowButton:NSWindowCloseButton];
	NSButton *miniaturizeBtn = [nsw standardWindowButton:NSWindowMiniaturizeButton];
	NSButton *zoomBtn = [nsw standardWindowButton:NSWindowZoomButton];

	closeBtn.translatesAutoresizingMaskIntoConstraints = NO;
	miniaturizeBtn.translatesAutoresizingMaskIntoConstraints = NO;
	zoomBtn.translatesAutoresizingMaskIntoConstraints = NO;

	NSLayoutConstraint *leftContraint1 = [NSLayoutConstraint constraintWithItem:closeBtn attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:closeBtn.superview attribute:NSLayoutAttributeLeft multiplier:1.0 constant:14.0];
	NSLayoutConstraint *topContraint1 = [NSLayoutConstraint constraintWithItem:closeBtn attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:closeBtn.superview attribute:NSLayoutAttributeCenterY multiplier:1.0 constant:0.0];
	leftContraint1.active = YES;
	topContraint1.active = YES;

	NSLayoutConstraint *leftContraint2 = [NSLayoutConstraint constraintWithItem:miniaturizeBtn attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:miniaturizeBtn.superview attribute:NSLayoutAttributeLeft multiplier:1.0 constant:33.0];
	NSLayoutConstraint *topContraint2 = [NSLayoutConstraint constraintWithItem:miniaturizeBtn attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:miniaturizeBtn.superview attribute:NSLayoutAttributeCenterY multiplier:1.0 constant:0.0];
	leftContraint2.active = YES;
	topContraint2.active = YES;

	NSLayoutConstraint *leftContraint3 = [NSLayoutConstraint constraintWithItem:zoomBtn attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:zoomBtn.superview attribute:NSLayoutAttributeLeft multiplier:1.0 constant:53.0];
	NSLayoutConstraint *topContraint3 = [NSLayoutConstraint constraintWithItem:zoomBtn attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:zoomBtn.superview attribute:NSLayoutAttributeCenterY multiplier:1.0 constant:0.0];
	leftContraint3.active = YES;
	topContraint3.active = YES;

	NSView *titleView = closeBtn.superview;
	NSView *titleBackgroundView = titleView.superview; // NSTitlebarContainerView
	//titleBackgroundView.layer.backgroundColor = CGColorCreateGenericGray(0.36, 1.0);
	titleBackgroundView.layer.backgroundColor = CGColorCreateGenericRGB(g_viewerSettings.bgColor[0], g_viewerSettings.bgColor[1], g_viewerSettings.bgColor[2], g_viewerSettings.bgColor[3]);

}

