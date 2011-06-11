//
//  SVTextDOMController.h
//  Marvel
//
//  Created by Mike on 22/08/2009.
//  Copyright 2009-2011 Karelia Software. All rights reserved.
//

//  An HTML element controller specialised to deal with text, generally of the editable variety. In a way, a bit like how NSText is too abstract to do anything on its own, but central to the operation of NSTextView and NSTextField. So check out the subclasses for more advanced stuff.


#import "SVDOMController.h"
#import "SVWebEditorTextProtocol.h"
#import "KSKeyValueBinding.h"
#import "SVGraphicContainerDOMController.h"

#import "SVParagraphedHTMLWriterDOMAdaptor.h"


@class SVHTMLTextBlock, SVGraphicDOMController;


@interface SVTextDOMController : SVDOMController <SVWebEditorText, SVGraphicContainerDOMController, DOMEventListener, KSXMLWriterDOMAdaptorDelegate>
{
  @private
    DOMHTMLElement  *_textElement;
    
    SVHTMLTextBlock *_textBlock;
    
    BOOL    _editable;
    BOOL    _isRichText;
    BOOL    _isFieldEditor;
    
    // Undo
    BOOL        _isCoalescingUndo;
    BOOL        _nextChangeIsSuitableForUndoCoalescing;
    NSUInteger  _undoCoalescingActionIdentifier;
    DOMRange    *_undoCoalescingSelection;
}


#pragma mark DOM Node

@property(nonatomic, retain) DOMHTMLElement *textHTMLElement;
- (BOOL)isTextHTMLElementLoaded;

- (DOMHTMLElement *)innerTextHTMLElement;  // default is -textHTMLElement. Subclasses override to nil if there isn't one or can't be found

- (WEKWebEditorItem *)orphanedWebEditorItemForImageDOMElement:(DOMHTMLImageElement *)aNode;


#pragma mark Properties

// NSTextView-like properties for controlling editing behaviour. Some are stored as part of the DOM, others as ivars. Note that some can only really take effect if properly hooked up to another controller that forwards on proper editing delegation methods from the WebView.
@property(nonatomic, getter=isEditable) BOOL editable;
@property(nonatomic, setter=setRichText:) BOOL isRichText;
@property(nonatomic, setter=setFieldEditor:) BOOL isFieldEditor;

@property(nonatomic, retain) SVHTMLTextBlock *textBlock;


#pragma mark Selection
// Like the Web Editor method, but returns nil if selection is outside receiver's text
- (DOMRange *)selectedDOMRange;


#pragma mark Editing

// e.g. Movement might be NSReturnTextMovement. Nil if we don't know
- (void)didEndEditingTextWithMovement:(NSNumber *)textMovement;

// Write the receiver's textual content using the passed in adaptor. Meant to happen for a change which updates the model
- (void)writeText:(KSXMLWriterDOMAdaptor *)adaptor;

// Subclassers take responsibility for overriding this to push to the model
- (void)setHTMLString:(NSString *)html attachments:(NSSet *)attachments;

- (id)newHTMLWritingDOMAdaptorWithOutputStringWriter:(KSStringWriter *)stringWriter;

+ (BOOL)isDeleteBackwardsSelector:(SEL)action;
+ (BOOL)isDeleteForwardsSelector:(SEL)action;


#pragma mark Undo

// The basic idea is that after every -didChange notification, the change should be pushed down to the model. BUT, if both the change and the previous one was a simple bit of typing, we don't want two separate undo operations recorded. So the undo is coalesced. That is to say, we take advantage of Core Data's behaviour by disabling undo registration during the change, which means the change effectively gets tacked onto the end of the previous one.

- (void)breakUndoCoalescing;

//  To implement this is actually a bit painful. We need some cooperation from subclasses and other infrastructure, which are:
//  
//      -   reference to the MOC so as to process changes when suits us, and access from there to undo manager
//
//      -   cooperation of the undo manager. NSTextView does its undo coalescing by inspecting the undo stack to see if the last op registered was by itself. We don't have that access, but can request that somebody else (*cough* the document) supply a suitable NSUndoManager subclass which gives an identifier for the item on top of the stack.

- (void)webEditorTextDidChange; // if you override, call super at end of method to get undo coalescing


@end



#pragma mark -


// See comments above for why this is necessary. You MUST supply a NSUndoManager subclass that implements this API properly for undo coalescing to work. Uses unsigned so that it can *never* reach NSNotFound, but will roll back round again instead
@interface NSUndoManager (SVWebEditorTextControllerUndoCoalescing)
- (unsigned short)lastRegisteredActionIdentifier;
@end


#pragma mark -


@interface WEKWebEditorItem (SVTextDOMController)
- (SVTextDOMController *)textDOMController; // seeks the closest ancestor text controller
@end


#pragma mark -


@interface WEKWebEditorItem (SVRichTextDOMController)

#pragma mark Properties
- (BOOL)allowsPagelets;

#pragma mark Attributed HTML
// Return YES if manages to write self. Otherwise return NO to treat as standard HTML
- (BOOL)writeAttributedHTML:(SVFieldEditorHTMLWriterDOMAdapator *)writer;

@end
