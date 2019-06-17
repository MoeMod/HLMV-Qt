//
// Created by MoeMod on 2019-06-18.
//

#import <AppKit/AppKit.h>
#import <QWindow>
#include "hlmv.h"

// This example shows how to create and populate touch bars for Qt applications.
// Two approaches are demonstrated: creating a global touch bar for the entire
// application via the NSApplication delegate, and creating per-window touch bars
// via the NSWindow delegate. Applications may use either or both of these, for example
// to provide global base touch bar with window specific additions. Refer to the
// NSTouchBar documentation for further details.

static NSTouchBarItemIdentifier TextItemIdentifier = @"com.myapp.TextItemIdentifier";

@interface ViewModeScrubberDelegate : NSResponder <NSScrubberDelegate, NSScrubberDataSource>
- (instancetype) initWithHLMV:(QtGuiApplication1 *)hlmv;
@property QtGuiApplication1 *hlmv;
@property (strong) NSArray<NSToolbarItem *> *validItems;
@end
@implementation ViewModeScrubberDelegate

- (instancetype) initWithHLMV:(QtGuiApplication1 *)hlmv {
	[super init];
	_hlmv = hlmv;
	NSWindow* nsw = [reinterpret_cast<NSView *>(self.hlmv->windowHandle()->winId()) window];
	NSToolbar *nst = nsw.toolbar;

	NSArray<NSToolbarItem *> *items = [nst.items copy];
	NSMutableArray<NSToolbarItem *> *new_items = [[NSMutableArray<NSToolbarItem *> alloc] init];

	int iSelected = 0;
	for(int i = 0; i < items.count; ++i)
	{
		NSToolbarItem * item = items[i];
		if(item.action == nil)
			continue;
		[new_items addObject:item];
		if([nst.selectedItemIdentifier isEqualToString:[item itemIdentifier]])
		{

		}
	}


	_validItems = new_items;

	return self;
}

- (NSInteger)numberOfItemsForScrubber:(NSScrubber *)scrubber {
	return [_validItems count];
}

- (NSScrubberItemView *)scrubber:(NSScrubber *)scrubber viewForItemAtIndex:(NSInteger)index {
	NSScrubberTextItemView *view = [scrubber makeItemWithIdentifier:TextItemIdentifier owner:nil];

	view.textField.stringValue = _validItems[index].label;

	return view;
}

- (NSSize)scrubber:(NSScrubber *)scrubber layout:(NSScrubberFlowLayout *)layout sizeForItemAtIndex:(NSInteger)index {
	QTabWidget *tabWidget = self.hlmv->ui.tabWidget;
	NSString *string = tabWidget->tabText(static_cast<int>(index)).toNSString();
	NSRect bounds = [string boundingRectWithSize:NSMakeSize(CGFLOAT_MAX, CGFLOAT_MAX)
	                                     options:NSStringDrawingUsesLineFragmentOrigin | NSStringDrawingUsesFontLeading
	                                  attributes:@{NSFontAttributeName: [NSFont systemFontOfSize:0]}];

	return NSMakeSize(bounds.size.width + 50, 30);
}

- (void)didFinishInteractingWithScrubber:(NSScrubber *)scrubber {

	auto selectedIndex = scrubber.selectedIndex;
	QTabWidget *tabWidget = self.hlmv->ui.tabWidget;

	NSWindow* nsw = [reinterpret_cast<NSView *>(self.hlmv->windowHandle()->winId()) window];
	NSToolbar *nst = nsw.toolbar;
	NSToolbarItem *nativeItem = _validItems[selectedIndex];
	[nst setSelectedItemIdentifier:[nativeItem itemIdentifier]];

	tabWidget->setCurrentWidget(tabWidget->widget(static_cast<int>(selectedIndex)));

}
- (void)dealloc {
	[_validItems release];
	[super dealloc];
}
@end

@interface SyncComboBoxScrubberDelegate : NSResponder <NSScrubberDelegate, NSScrubberDataSource>
- (instancetype) initWithComboBox:(QComboBox *)combo;
@property QComboBox *combo;
@end
@implementation SyncComboBoxScrubberDelegate

- (instancetype) initWithComboBox:(QComboBox *)combo {
	[super init];
	_combo = combo;

	return self;
}

- (NSInteger)numberOfItemsForScrubber:(NSScrubber *)scrubber {
	return _combo->count();
}

- (NSScrubberItemView *)scrubber:(NSScrubber *)scrubber viewForItemAtIndex:(NSInteger)index {
	NSScrubberTextItemView *view = [scrubber makeItemWithIdentifier:TextItemIdentifier owner:nil];
	view.title = _combo->itemText(index).toNSString();
	return view;
}

- (NSSize)scrubber:(NSScrubber *)scrubber layout:(NSScrubberFlowLayout *)layout sizeForItemAtIndex:(NSInteger)index {
	NSString *string = _combo->itemText(index).toNSString();
	NSRect bounds = [string boundingRectWithSize:NSMakeSize(CGFLOAT_MAX, CGFLOAT_MAX)
	                                     options:NSStringDrawingUsesLineFragmentOrigin | NSStringDrawingUsesFontLeading
	                                  attributes:@{NSFontAttributeName: [NSFont systemFontOfSize:0]}];

	return NSMakeSize(bounds.size.width + 20, 30);
}

- (void)scrubber:(NSScrubber *)scrubber didSelectItemAtIndex:(NSInteger)selectedIndex {

	_combo->setCurrentIndex(selectedIndex);

}
@end

@interface SyncComboBoxScrubberTouchBarItem: NSCustomTouchBarItem
- (instancetype) initWithIdentifier:identifier ComboBox:(QComboBox *)combo;
@property (strong) NSScrubber *scrubber;
@property QComboBox *combo;
@property QMetaObject::Connection connection;
@end
@implementation SyncComboBoxScrubberTouchBarItem

- (instancetype) initWithIdentifier:identifier ComboBox:(QComboBox *)combo {
	self = [super initWithIdentifier:identifier];
	NSScrubber *scrubber = [[[NSScrubber alloc] init] autorelease];
	scrubber.scrubberLayout = [[NSScrubberFlowLayout alloc] init];
	scrubber.mode = NSScrubberModeFree;
	scrubber.continuous = false;
	scrubber.selectionBackgroundStyle = [NSScrubberSelectionStyle outlineOverlayStyle];

	SyncComboBoxScrubberDelegate *scrubberDelegate = [[SyncComboBoxScrubberDelegate alloc] initWithComboBox:combo];
	scrubber.delegate = scrubberDelegate;
	scrubber.dataSource = scrubberDelegate;
	scrubber.floatsSelectionViews = false;
	scrubber.selectedIndex = combo->currentIndex();

	[scrubber registerClass:[NSScrubberTextItemView class] forItemIdentifier:TextItemIdentifier];

	self.view = _scrubber = scrubber;
	_combo = combo;
	_connection = QObject::connect(combo,
			static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
			[self]{ _scrubber.selectedIndex =_combo->currentIndex(); }
			);
	return self;
}

@end

// The TouchBarProvider class implements the NSTouchBarDelegate protocol, as
// well as app and window delegate protocols.
@interface TouchBarProvider: NSResponder <NSTouchBarDelegate, NSApplicationDelegate, NSWindowDelegate>

@property (strong) NSCustomTouchBarItem *touchBarItem1;
@property (strong) NSCustomTouchBarItem *touchBarItem2;
@property (strong) NSCustomTouchBarItem *touchBarItem3;
@property (strong) NSCustomTouchBarItem *touchBarItem4;

@property (strong) NSButton *touchBarButton1;
@property (strong) NSButton *touchBarButton2;
@property (strong) NSButton *touchBarButton3;
@property (strong) NSButton *touchBarButton4;

@property (strong) NSObject *qtDelegate;

@property QtGuiApplication1 *hlmv;

- (instancetype) initWithHLMVInstance:(QtGuiApplication1 *)p;

@end

// Create identifiers for two button items.
static NSTouchBarItemIdentifier Button1Identifier = @"com.myapp.Button1Identifier";
static NSTouchBarItemIdentifier Button2Identifier = @"com.myapp.Button2Identifier";
static NSTouchBarItemIdentifier Button3Identifier = @"com.myapp.Button3Identifier";
static NSTouchBarItemIdentifier Button4Identifier = @"com.myapp.Button4Identifier";
static NSTouchBarItemIdentifier ViewModeScrubberItemIdentifier = @"com.hlmv.ViewModeScrubberItemIdentifier";
static NSTouchBarItemIdentifier ViewModeItemIdentifier = @"com.hlmv.ViewModeItemIdentifier";
static NSTouchBarItemIdentifier SequenceScrubberItemIdentifier = @"com.hlmv.SequenceScrubberItemIdentifier";
static NSTouchBarItemIdentifier TextureItemIdentifier = @"com.hlmv.TextureItemIdentifier";
static NSTouchBarItemIdentifier TextureScrubberItemIdentifier = @"com.hlmv.TextureScrubberItemIdentifier";
static NSTouchBarItemIdentifier BodyPartItemIdentifier = @"com.hlmv.BodyPartItemIdentifier";
static NSTouchBarItemIdentifier BodyPartScrubberItemIdentifier = @"com.hlmv.BodyPartScrubberItemIdentifier";
static NSTouchBarItemIdentifier SubModelItemIdentifier = @"com.hlmv.SubModelItemIdentifier";
static NSTouchBarItemIdentifier SubModelScrubberItemIdentifier = @"com.hlmv.SubModelScrubberItemIdentifier";
static NSTouchBarItemIdentifier SkinItemIdentifier = @"com.hlmv.SkinItemIdentifier";
static NSTouchBarItemIdentifier SkinScrubberItemIdentifier = @"com.hlmv.SkinScrubberItemIdentifier";

@implementation TouchBarProvider

- (instancetype) initWithHLMVInstance:(QtGuiApplication1 *)p
{
	self = [super init];

	self.hlmv = p;

	return self;
}

- (NSTouchBar *)makeTouchBar
{
	// Create the touch bar with this instance as its delegate
	NSTouchBar *bar = [[NSTouchBar alloc] init];
	bar.delegate = self;

	// Add touch bar items: first, the very important emoji picker, followed
	// by two buttons. Note that no further handling of the emoji picker
	// is needed (emojii are automatically routed to any active text edit). Button
	// actions handlers are set up in makeItemForIdentifier below.
	if(_hlmv->ui.tabWidget->currentIndex() == 3 || _hlmv->ui.tabWidget->currentIndex() == 4) // view origin
	{
		bar.defaultItemIdentifiers = @[\
			ViewModeItemIdentifier,
		    NSTouchBarItemIdentifierFixedSpaceLarge,
		    SequenceScrubberItemIdentifier
		];
	}
	else if(_hlmv->ui.tabWidget->currentIndex() == 2) // texture
	{
		bar.defaultItemIdentifiers = @[\
			ViewModeItemIdentifier,
		    NSTouchBarItemIdentifierFixedSpaceLarge,
		    TextureItemIdentifier
		];
	}
	else if(_hlmv->ui.tabWidget->currentIndex() == 1) // body part
	{
		bar.defaultItemIdentifiers = @[\
			ViewModeItemIdentifier,
		    NSTouchBarItemIdentifierFixedSpaceLarge,
		    BodyPartItemIdentifier,
		    SubModelItemIdentifier,
		    SkinItemIdentifier,
		];
	}
	else
	{
		bar.defaultItemIdentifiers = @[\
			ViewModeScrubberItemIdentifier
		];
	}


	return bar;
}

- (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier
{
	Q_UNUSED(touchBar);

	// Create touch bar items as NSCustomTouchBarItems which can contain any NSView.
	if ([identifier isEqualToString:ViewModeItemIdentifier]) {
		NSPopoverTouchBarItem *item = [[NSPopoverTouchBarItem alloc] initWithIdentifier:identifier];

		QTabBar *tabBar = _hlmv->ui.tabWidget->tabBar();
		item.collapsedRepresentationLabel = tabBar->tabText(tabBar->currentIndex()).toNSString();
		item.collapsedRepresentationImage = [NSImage imageNamed:NSImageNameTouchBarSidebarTemplate];

		NSTouchBar *secondaryTouchBar = [[NSTouchBar alloc] init];
		secondaryTouchBar.delegate = self;
		secondaryTouchBar.defaultItemIdentifiers = @[ViewModeScrubberItemIdentifier];

		item.popoverTouchBar = secondaryTouchBar;

		return item;
	}
	else if ([identifier isEqualToString:ViewModeScrubberItemIdentifier]) {

		ViewModeScrubberDelegate *scrubberDelegate = [[ViewModeScrubberDelegate alloc] initWithHLMV:_hlmv];
		//scrubberDelegate.hlmv = hlmv;

		NSScrubber *scrubber = [[[NSScrubber alloc] init] autorelease];
		scrubber.scrubberLayout = [[NSScrubberFlowLayout alloc] init];
		//scrubber.mode = NSScrubberModeFree;
		scrubber.continuous = true;
		scrubber.selectionBackgroundStyle = [NSScrubberSelectionStyle outlineOverlayStyle];
		scrubber.delegate = scrubberDelegate;
		scrubber.dataSource = scrubberDelegate;
		scrubber.floatsSelectionViews = false;
		scrubber.selectedIndex = _hlmv->ui.tabWidget->currentIndex();

		[scrubber registerClass:[NSScrubberTextItemView class] forItemIdentifier:TextItemIdentifier];


		NSCustomTouchBarItem *item = [[NSCustomTouchBarItem alloc] initWithIdentifier:identifier];
		item.view = scrubber;

		return item;
	}
	else if ([identifier isEqualToString:TextureItemIdentifier]) {
		NSPopoverTouchBarItem *item = [[NSPopoverTouchBarItem alloc] initWithIdentifier:identifier];
		item.collapsedRepresentationLabel = _hlmv->ui.cTextures->currentText().toNSString();
		item.collapsedRepresentationImage = [NSImage imageNamed:NSImageNameTouchBarQuickLookTemplate];
		NSTouchBar *secondaryTouchBar = [[NSTouchBar alloc] init];
		secondaryTouchBar.delegate = self;
		secondaryTouchBar.defaultItemIdentifiers = @[TextureScrubberItemIdentifier];
		item.popoverTouchBar = secondaryTouchBar;
		return item;
	}
	else if ([identifier isEqualToString:TextureScrubberItemIdentifier]) {
		return [[SyncComboBoxScrubberTouchBarItem alloc] initWithIdentifier:identifier ComboBox:_hlmv->ui.cTextures];
	}
	else if ([identifier isEqualToString:BodyPartItemIdentifier]) {
		NSPopoverTouchBarItem *item = [[NSPopoverTouchBarItem alloc] initWithIdentifier:identifier];
		item.collapsedRepresentationLabel = _hlmv->ui.cBodypart->currentText().toNSString();
		item.collapsedRepresentationImage = [NSImage imageNamed:NSImageNameTouchBarColorPickerFill];
		NSTouchBar *secondaryTouchBar = [[NSTouchBar alloc] init];
		secondaryTouchBar.delegate = self;
		secondaryTouchBar.defaultItemIdentifiers = @[BodyPartScrubberItemIdentifier];
		item.popoverTouchBar = secondaryTouchBar;
		return item;
	}
	else if ([identifier isEqualToString:BodyPartScrubberItemIdentifier]) {
		return [[SyncComboBoxScrubberTouchBarItem alloc] initWithIdentifier:identifier ComboBox:_hlmv->ui.cBodypart];
	}
	else if ([identifier isEqualToString:SubModelItemIdentifier]) {
		NSPopoverTouchBarItem *item = [[NSPopoverTouchBarItem alloc] initWithIdentifier:identifier];
		item.collapsedRepresentationLabel = _hlmv->ui.cSubmodel->currentText().toNSString();
		item.collapsedRepresentationImage = [NSImage imageNamed:NSImageNameTouchBarRecordStartTemplate];
		NSTouchBar *secondaryTouchBar = [[NSTouchBar alloc] init];
		secondaryTouchBar.delegate = self;
		secondaryTouchBar.defaultItemIdentifiers = @[SubModelScrubberItemIdentifier];
		item.popoverTouchBar = secondaryTouchBar;
		return item;
	}
	else if ([identifier isEqualToString:SubModelScrubberItemIdentifier]) {
		return [[SyncComboBoxScrubberTouchBarItem alloc] initWithIdentifier:identifier ComboBox:_hlmv->ui.cSubmodel];
	}
	else if ([identifier isEqualToString:SkinItemIdentifier]) {
		NSPopoverTouchBarItem *item = [[NSPopoverTouchBarItem alloc] initWithIdentifier:identifier];
		item.collapsedRepresentationLabel = _hlmv->ui.cSkin->currentText().toNSString();
		item.collapsedRepresentationImage = [NSImage imageNamed:NSImageNameTouchBarColorPickerStroke];
		NSTouchBar *secondaryTouchBar = [[NSTouchBar alloc] init];
		secondaryTouchBar.delegate = self;
		secondaryTouchBar.defaultItemIdentifiers = @[SkinScrubberItemIdentifier];
		item.popoverTouchBar = secondaryTouchBar;
		return item;
	}
	else if ([identifier isEqualToString:SkinScrubberItemIdentifier]) {
		return [[SyncComboBoxScrubberTouchBarItem alloc] initWithIdentifier:identifier ComboBox:_hlmv->ui.cSkin];
	}
	else if ([identifier isEqualToString:SequenceScrubberItemIdentifier]) {
		return [[SyncComboBoxScrubberTouchBarItem alloc] initWithIdentifier:identifier ComboBox:_hlmv->ui.cWpSequence];
	}
	else if ([identifier isEqualToString:Button1Identifier]) {
		QString title = "Welcome";
		self.touchBarItem1 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
		self.touchBarButton1 = [[NSButton buttonWithTitle:title.toNSString() target:self
		                                           action:@selector(button1Clicked)] autorelease];
		self.touchBarButton1.imageHugsTitle = true;
		self.touchBarItem1.view =  self.touchBarButton1;
		return self.touchBarItem1;
	}
	else if ([identifier isEqualToString:Button2Identifier]) {
		QString title = "New Game";
		self.touchBarItem2 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
		self.touchBarButton2 = [[NSButton buttonWithTitle:title.toNSString() target:self
		                                           action:@selector(button2Clicked)] autorelease];
		self.touchBarButton2.imageHugsTitle = true;
		self.touchBarItem2.view =  self.touchBarButton2;
		return self.touchBarItem2;
	}
	else if ([identifier isEqualToString:Button3Identifier]) {
		QString title = "Servers";
		self.touchBarItem3 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
		self.touchBarButton3 = [[NSButton buttonWithTitle:title.toNSString() target:self
		                                           action:@selector(button3Clicked)] autorelease];
		self.touchBarButton3.imageHugsTitle = true;
		self.touchBarItem3.view =  self.touchBarButton3;
		return self.touchBarItem3;
	}
	else if ([identifier isEqualToString:Button4Identifier]) {
		QString title = "Settings";
		self.touchBarItem4 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
		self.touchBarButton4 = [[NSButton buttonWithTitle:title.toNSString() target:self
		                                           action:@selector(button4Clicked)] autorelease];
		self.touchBarButton4.imageHugsTitle = true;
		self.touchBarItem4.view =  self.touchBarButton4;
		return self.touchBarItem4;
	}
	return nil;
}

- (void)installAsDelegateForWindow:(NSWindow *)window
{
	_qtDelegate = window.delegate; // Save current delegate for forwarding
	window.delegate = self;
}

- (void)installAsDelegateForApplication:(NSApplication *)application
{
	_qtDelegate = application.delegate; // Save current delegate for forwarding
	application.delegate = self;
}

- (BOOL)respondsToSelector:(SEL)aSelector
{
	// We want to forward to the qt delegate. Respond to selectors it
	// responds to in addition to selectors this instance resonds to.
	return [_qtDelegate respondsToSelector:aSelector] || [super respondsToSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
	// Forward to the existing delegate. This function is only called for selectors
	// this instance does not responds to, which means that the qt delegate
	// must respond to it (due to the respondsToSelector implementation above).
	[anInvocation invokeWithTarget:_qtDelegate];
}

- (void)button1Clicked
{
	[[NSApplication sharedApplication] toggleTouchBarCustomizationPalette:nil];

}

- (void)button2Clicked
{

}

- (void)button3Clicked
{

}

- (void)button4Clicked
{

}

@end

void QtGuiApplication1::InstallTouchBar()
{
	NSWindow* nsw = [reinterpret_cast<NSView *>(this->windowHandle()->winId()) window];
	if(nsw.touchBar)
	{
		nsw.touchBar = nil;
	}
	else
	{
		// Install TouchBarProvider as application delegate
		TouchBarProvider *touchBarProvider = [[TouchBarProvider alloc] initWithHLMVInstance:this];
		//[touchBarProvider installAsDelegateForApplication:[NSApplication sharedApplication]];
		[touchBarProvider installAsDelegateForWindow:nsw];
	}
}
