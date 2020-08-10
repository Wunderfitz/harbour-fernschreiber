#pragma once

#include "td/tl/TlObject.h"

#include <string>

#include <cstdint>
#include <utility>
#include <vector>

namespace td {
class TlStorerToString;

namespace td_api {

using BaseObject = ::td::TlObject;

template <class Type>
using object_ptr = ::td::tl_object_ptr<Type>;

template <class Type, class... Args>
object_ptr<Type> make_object(Args &&... args) {
  return object_ptr<Type>(new Type(std::forward<Args>(args)...));
}

template <class ToType, class FromType>
object_ptr<ToType> move_object_as(FromType &&from) {
  return object_ptr<ToType>(static_cast<ToType *>(from.release()));
}

std::string to_string(const BaseObject &value);

template <class T>
std::string to_string(const object_ptr<T> &value) {
  if (value == nullptr) {
    return "null";
  }

  return to_string(*value);
}

class accountTtl;

class address;

class animation;

class animations;

class audio;

class authenticationCodeInfo;

class AuthenticationCodeType;

class AuthorizationState;

class autoDownloadSettings;

class autoDownloadSettingsPresets;

class background;

class BackgroundFill;

class BackgroundType;

class backgrounds;

class basicGroup;

class basicGroupFullInfo;

class botCommand;

class botInfo;

class call;

class callConnection;

class CallDiscardReason;

class callId;

class CallProblem;

class callProtocol;

class CallState;

class callbackQueryAnswer;

class CallbackQueryPayload;

class CanTransferOwnershipResult;

class chat;

class ChatAction;

class ChatActionBar;

class chatAdministrator;

class chatAdministrators;

class chatEvent;

class ChatEventAction;

class chatEventLogFilters;

class chatEvents;

class chatInviteLink;

class chatInviteLinkInfo;

class ChatList;

class chatLocation;

class chatMember;

class ChatMemberStatus;

class chatMembers;

class ChatMembersFilter;

class chatNearby;

class chatNotificationSettings;

class chatPermissions;

class chatPhoto;

class ChatReportReason;

class ChatType;

class chats;

class chatsNearby;

class CheckChatUsernameResult;

class connectedWebsite;

class connectedWebsites;

class ConnectionState;

class contact;

class count;

class customRequestResult;

class databaseStatistics;

class date;

class datedFile;

class deepLinkInfo;

class DeviceToken;

class document;

class draftMessage;

class emailAddressAuthenticationCodeInfo;

class emojis;

class encryptedCredentials;

class encryptedPassportElement;

class error;

class file;

class filePart;

class FileType;

class formattedText;

class foundMessages;

class game;

class gameHighScore;

class gameHighScores;

class hashtags;

class httpUrl;

class identityDocument;

class importedContacts;

class inlineKeyboardButton;

class InlineKeyboardButtonType;

class InlineQueryResult;

class inlineQueryResults;

class InputBackground;

class InputCredentials;

class InputFile;

class inputIdentityDocument;

class InputInlineQueryResult;

class InputMessageContent;

class InputPassportElement;

class inputPassportElementError;

class InputPassportElementErrorSource;

class inputPersonalDocument;

class inputSticker;

class inputThumbnail;

class invoice;

class jsonObjectMember;

class JsonValue;

class keyboardButton;

class KeyboardButtonType;

class labeledPricePart;

class languagePackInfo;

class languagePackString;

class LanguagePackStringValue;

class languagePackStrings;

class localFile;

class localizationTargetInfo;

class location;

class LogStream;

class logTags;

class logVerbosityLevel;

class LoginUrlInfo;

class MaskPoint;

class maskPosition;

class message;

class MessageContent;

class messageForwardInfo;

class MessageForwardOrigin;

class messageLinkInfo;

class MessageSchedulingState;

class MessageSendingState;

class messages;

class minithumbnail;

class networkStatistics;

class NetworkStatisticsEntry;

class NetworkType;

class notification;

class notificationGroup;

class NotificationGroupType;

class NotificationSettingsScope;

class NotificationType;

class ok;

class OptionValue;

class orderInfo;

class PageBlock;

class pageBlockCaption;

class PageBlockHorizontalAlignment;

class pageBlockListItem;

class pageBlockRelatedArticle;

class pageBlockTableCell;

class PageBlockVerticalAlignment;

class passportAuthorizationForm;

class PassportElement;

class passportElementError;

class PassportElementErrorSource;

class PassportElementType;

class passportElements;

class passportElementsWithErrors;

class passportRequiredElement;

class passportSuitableElement;

class passwordState;

class paymentForm;

class paymentReceipt;

class paymentResult;

class paymentsProviderStripe;

class personalDetails;

class personalDocument;

class phoneNumberAuthenticationSettings;

class photo;

class photoSize;

class poll;

class pollOption;

class PollType;

class profilePhoto;

class proxies;

class proxy;

class ProxyType;

class PublicChatType;

class publicMessageLink;

class PushMessageContent;

class pushReceiverId;

class recoveryEmailAddress;

class remoteFile;

class ReplyMarkup;

class RichText;

class savedCredentials;

class scopeNotificationSettings;

class SearchMessagesFilter;

class seconds;

class secretChat;

class SecretChatState;

class sendMessageOptions;

class session;

class sessions;

class shippingOption;

class sticker;

class stickerSet;

class stickerSetInfo;

class stickerSets;

class stickers;

class storageStatistics;

class storageStatisticsByChat;

class storageStatisticsByFileType;

class storageStatisticsFast;

class supergroup;

class supergroupFullInfo;

class SupergroupMembersFilter;

class tMeUrl;

class TMeUrlType;

class tMeUrls;

class tdlibParameters;

class temporaryPasswordState;

class termsOfService;

class testBytes;

class testInt;

class testString;

class testVectorInt;

class testVectorIntObject;

class testVectorString;

class testVectorStringObject;

class text;

class textEntities;

class textEntity;

class TextEntityType;

class TextParseMode;

class TopChatCategory;

class Update;

class updates;

class user;

class userFullInfo;

class UserPrivacySetting;

class UserPrivacySettingRule;

class userPrivacySettingRules;

class userProfilePhoto;

class userProfilePhotos;

class UserStatus;

class UserType;

class users;

class validatedOrderInfo;

class venue;

class video;

class videoNote;

class voiceNote;

class webPage;

class webPageInstantView;

class Object;

class Object: public TlObject {
 public:
};

class Function: public TlObject {
 public:
};

class accountTtl final : public Object {
 public:
  std::int32_t days_;

  accountTtl();

  explicit accountTtl(std::int32_t days_);

  static const std::int32_t ID = 1324495492;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class address final : public Object {
 public:
  std::string country_code_;
  std::string state_;
  std::string city_;
  std::string street_line1_;
  std::string street_line2_;
  std::string postal_code_;

  address();

  address(std::string const &country_code_, std::string const &state_, std::string const &city_, std::string const &street_line1_, std::string const &street_line2_, std::string const &postal_code_);

  static const std::int32_t ID = -2043654342;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class animation final : public Object {
 public:
  std::int32_t duration_;
  std::int32_t width_;
  std::int32_t height_;
  std::string file_name_;
  std::string mime_type_;
  object_ptr<minithumbnail> minithumbnail_;
  object_ptr<photoSize> thumbnail_;
  object_ptr<file> animation_;

  animation();

  animation(std::int32_t duration_, std::int32_t width_, std::int32_t height_, std::string const &file_name_, std::string const &mime_type_, object_ptr<minithumbnail> &&minithumbnail_, object_ptr<photoSize> &&thumbnail_, object_ptr<file> &&animation_);

  static const std::int32_t ID = -1629245379;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class animations final : public Object {
 public:
  std::vector<object_ptr<animation>> animations_;

  animations();

  explicit animations(std::vector<object_ptr<animation>> &&animations_);

  static const std::int32_t ID = 344216945;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class audio final : public Object {
 public:
  std::int32_t duration_;
  std::string title_;
  std::string performer_;
  std::string file_name_;
  std::string mime_type_;
  object_ptr<minithumbnail> album_cover_minithumbnail_;
  object_ptr<photoSize> album_cover_thumbnail_;
  object_ptr<file> audio_;

  audio();

  audio(std::int32_t duration_, std::string const &title_, std::string const &performer_, std::string const &file_name_, std::string const &mime_type_, object_ptr<minithumbnail> &&album_cover_minithumbnail_, object_ptr<photoSize> &&album_cover_thumbnail_, object_ptr<file> &&audio_);

  static const std::int32_t ID = 1475294302;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authenticationCodeInfo final : public Object {
 public:
  std::string phone_number_;
  object_ptr<AuthenticationCodeType> type_;
  object_ptr<AuthenticationCodeType> next_type_;
  std::int32_t timeout_;

  authenticationCodeInfo();

  authenticationCodeInfo(std::string const &phone_number_, object_ptr<AuthenticationCodeType> &&type_, object_ptr<AuthenticationCodeType> &&next_type_, std::int32_t timeout_);

  static const std::int32_t ID = -860345416;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class AuthenticationCodeType: public Object {
 public:
};

class authenticationCodeTypeTelegramMessage final : public AuthenticationCodeType {
 public:
  std::int32_t length_;

  authenticationCodeTypeTelegramMessage();

  explicit authenticationCodeTypeTelegramMessage(std::int32_t length_);

  static const std::int32_t ID = 2079628074;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authenticationCodeTypeSms final : public AuthenticationCodeType {
 public:
  std::int32_t length_;

  authenticationCodeTypeSms();

  explicit authenticationCodeTypeSms(std::int32_t length_);

  static const std::int32_t ID = 962650760;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authenticationCodeTypeCall final : public AuthenticationCodeType {
 public:
  std::int32_t length_;

  authenticationCodeTypeCall();

  explicit authenticationCodeTypeCall(std::int32_t length_);

  static const std::int32_t ID = 1636265063;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authenticationCodeTypeFlashCall final : public AuthenticationCodeType {
 public:
  std::string pattern_;

  authenticationCodeTypeFlashCall();

  explicit authenticationCodeTypeFlashCall(std::string const &pattern_);

  static const std::int32_t ID = 1395882402;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class AuthorizationState: public Object {
 public:
};

class authorizationStateWaitTdlibParameters final : public AuthorizationState {
 public:

  authorizationStateWaitTdlibParameters();

  static const std::int32_t ID = 904720988;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitEncryptionKey final : public AuthorizationState {
 public:
  bool is_encrypted_;

  authorizationStateWaitEncryptionKey();

  explicit authorizationStateWaitEncryptionKey(bool is_encrypted_);

  static const std::int32_t ID = 612103496;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitPhoneNumber final : public AuthorizationState {
 public:

  authorizationStateWaitPhoneNumber();

  static const std::int32_t ID = 306402531;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitCode final : public AuthorizationState {
 public:
  object_ptr<authenticationCodeInfo> code_info_;

  authorizationStateWaitCode();

  explicit authorizationStateWaitCode(object_ptr<authenticationCodeInfo> &&code_info_);

  static const std::int32_t ID = 52643073;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitOtherDeviceConfirmation final : public AuthorizationState {
 public:
  std::string link_;

  authorizationStateWaitOtherDeviceConfirmation();

  explicit authorizationStateWaitOtherDeviceConfirmation(std::string const &link_);

  static const std::int32_t ID = 860166378;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitRegistration final : public AuthorizationState {
 public:
  object_ptr<termsOfService> terms_of_service_;

  authorizationStateWaitRegistration();

  explicit authorizationStateWaitRegistration(object_ptr<termsOfService> &&terms_of_service_);

  static const std::int32_t ID = 550350511;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateWaitPassword final : public AuthorizationState {
 public:
  std::string password_hint_;
  bool has_recovery_email_address_;
  std::string recovery_email_address_pattern_;

  authorizationStateWaitPassword();

  authorizationStateWaitPassword(std::string const &password_hint_, bool has_recovery_email_address_, std::string const &recovery_email_address_pattern_);

  static const std::int32_t ID = 187548796;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateReady final : public AuthorizationState {
 public:

  authorizationStateReady();

  static const std::int32_t ID = -1834871737;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateLoggingOut final : public AuthorizationState {
 public:

  authorizationStateLoggingOut();

  static const std::int32_t ID = 154449270;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateClosing final : public AuthorizationState {
 public:

  authorizationStateClosing();

  static const std::int32_t ID = 445855311;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class authorizationStateClosed final : public AuthorizationState {
 public:

  authorizationStateClosed();

  static const std::int32_t ID = 1526047584;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class autoDownloadSettings final : public Object {
 public:
  bool is_auto_download_enabled_;
  std::int32_t max_photo_file_size_;
  std::int32_t max_video_file_size_;
  std::int32_t max_other_file_size_;
  std::int32_t video_upload_bitrate_;
  bool preload_large_videos_;
  bool preload_next_audio_;
  bool use_less_data_for_calls_;

  autoDownloadSettings();

  autoDownloadSettings(bool is_auto_download_enabled_, std::int32_t max_photo_file_size_, std::int32_t max_video_file_size_, std::int32_t max_other_file_size_, std::int32_t video_upload_bitrate_, bool preload_large_videos_, bool preload_next_audio_, bool use_less_data_for_calls_);

  static const std::int32_t ID = -2144418333;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class autoDownloadSettingsPresets final : public Object {
 public:
  object_ptr<autoDownloadSettings> low_;
  object_ptr<autoDownloadSettings> medium_;
  object_ptr<autoDownloadSettings> high_;

  autoDownloadSettingsPresets();

  autoDownloadSettingsPresets(object_ptr<autoDownloadSettings> &&low_, object_ptr<autoDownloadSettings> &&medium_, object_ptr<autoDownloadSettings> &&high_);

  static const std::int32_t ID = -782099166;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class background final : public Object {
 public:
  std::int64_t id_;
  bool is_default_;
  bool is_dark_;
  std::string name_;
  object_ptr<document> document_;
  object_ptr<BackgroundType> type_;

  background();

  background(std::int64_t id_, bool is_default_, bool is_dark_, std::string const &name_, object_ptr<document> &&document_, object_ptr<BackgroundType> &&type_);

  static const std::int32_t ID = -429971172;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class BackgroundFill: public Object {
 public:
};

class backgroundFillSolid final : public BackgroundFill {
 public:
  std::int32_t color_;

  backgroundFillSolid();

  explicit backgroundFillSolid(std::int32_t color_);

  static const std::int32_t ID = 1010678813;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class backgroundFillGradient final : public BackgroundFill {
 public:
  std::int32_t top_color_;
  std::int32_t bottom_color_;
  std::int32_t rotation_angle_;

  backgroundFillGradient();

  backgroundFillGradient(std::int32_t top_color_, std::int32_t bottom_color_, std::int32_t rotation_angle_);

  static const std::int32_t ID = -1839206017;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class BackgroundType: public Object {
 public:
};

class backgroundTypeWallpaper final : public BackgroundType {
 public:
  bool is_blurred_;
  bool is_moving_;

  backgroundTypeWallpaper();

  backgroundTypeWallpaper(bool is_blurred_, bool is_moving_);

  static const std::int32_t ID = 1972128891;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class backgroundTypePattern final : public BackgroundType {
 public:
  object_ptr<BackgroundFill> fill_;
  std::int32_t intensity_;
  bool is_moving_;

  backgroundTypePattern();

  backgroundTypePattern(object_ptr<BackgroundFill> &&fill_, std::int32_t intensity_, bool is_moving_);

  static const std::int32_t ID = 649993914;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class backgroundTypeFill final : public BackgroundType {
 public:
  object_ptr<BackgroundFill> fill_;

  backgroundTypeFill();

  explicit backgroundTypeFill(object_ptr<BackgroundFill> &&fill_);

  static const std::int32_t ID = 993008684;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class backgrounds final : public Object {
 public:
  std::vector<object_ptr<background>> backgrounds_;

  backgrounds();

  explicit backgrounds(std::vector<object_ptr<background>> &&backgrounds_);

  static const std::int32_t ID = 724728704;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class basicGroup final : public Object {
 public:
  std::int32_t id_;
  std::int32_t member_count_;
  object_ptr<ChatMemberStatus> status_;
  bool is_active_;
  std::int32_t upgraded_to_supergroup_id_;

  basicGroup();

  basicGroup(std::int32_t id_, std::int32_t member_count_, object_ptr<ChatMemberStatus> &&status_, bool is_active_, std::int32_t upgraded_to_supergroup_id_);

  static const std::int32_t ID = -317839045;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class basicGroupFullInfo final : public Object {
 public:
  std::string description_;
  std::int32_t creator_user_id_;
  std::vector<object_ptr<chatMember>> members_;
  std::string invite_link_;

  basicGroupFullInfo();

  basicGroupFullInfo(std::string const &description_, std::int32_t creator_user_id_, std::vector<object_ptr<chatMember>> &&members_, std::string const &invite_link_);

  static const std::int32_t ID = 161500149;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class botCommand final : public Object {
 public:
  std::string command_;
  std::string description_;

  botCommand();

  botCommand(std::string const &command_, std::string const &description_);

  static const std::int32_t ID = -1032140601;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class botInfo final : public Object {
 public:
  std::string description_;
  std::vector<object_ptr<botCommand>> commands_;

  botInfo();

  botInfo(std::string const &description_, std::vector<object_ptr<botCommand>> &&commands_);

  static const std::int32_t ID = 1296528907;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class call final : public Object {
 public:
  std::int32_t id_;
  std::int32_t user_id_;
  bool is_outgoing_;
  object_ptr<CallState> state_;

  call();

  call(std::int32_t id_, std::int32_t user_id_, bool is_outgoing_, object_ptr<CallState> &&state_);

  static const std::int32_t ID = -1837599107;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callConnection final : public Object {
 public:
  std::int64_t id_;
  std::string ip_;
  std::string ipv6_;
  std::int32_t port_;
  std::string peer_tag_;

  callConnection();

  callConnection(std::int64_t id_, std::string const &ip_, std::string const &ipv6_, std::int32_t port_, std::string const &peer_tag_);

  static const std::int32_t ID = 1318542714;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CallDiscardReason: public Object {
 public:
};

class callDiscardReasonEmpty final : public CallDiscardReason {
 public:

  callDiscardReasonEmpty();

  static const std::int32_t ID = -1258917949;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callDiscardReasonMissed final : public CallDiscardReason {
 public:

  callDiscardReasonMissed();

  static const std::int32_t ID = 1680358012;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callDiscardReasonDeclined final : public CallDiscardReason {
 public:

  callDiscardReasonDeclined();

  static const std::int32_t ID = -1729926094;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callDiscardReasonDisconnected final : public CallDiscardReason {
 public:

  callDiscardReasonDisconnected();

  static const std::int32_t ID = -1342872670;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callDiscardReasonHungUp final : public CallDiscardReason {
 public:

  callDiscardReasonHungUp();

  static const std::int32_t ID = 438216166;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callId final : public Object {
 public:
  std::int32_t id_;

  callId();

  explicit callId(std::int32_t id_);

  static const std::int32_t ID = 65717769;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CallProblem: public Object {
 public:
};

class callProblemEcho final : public CallProblem {
 public:

  callProblemEcho();

  static const std::int32_t ID = 801116548;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemNoise final : public CallProblem {
 public:

  callProblemNoise();

  static const std::int32_t ID = 1053065359;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemInterruptions final : public CallProblem {
 public:

  callProblemInterruptions();

  static const std::int32_t ID = 1119493218;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemDistortedSpeech final : public CallProblem {
 public:

  callProblemDistortedSpeech();

  static const std::int32_t ID = 379960581;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemSilentLocal final : public CallProblem {
 public:

  callProblemSilentLocal();

  static const std::int32_t ID = 253652790;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemSilentRemote final : public CallProblem {
 public:

  callProblemSilentRemote();

  static const std::int32_t ID = 573634714;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProblemDropped final : public CallProblem {
 public:

  callProblemDropped();

  static const std::int32_t ID = -1207311487;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callProtocol final : public Object {
 public:
  bool udp_p2p_;
  bool udp_reflector_;
  std::int32_t min_layer_;
  std::int32_t max_layer_;

  callProtocol();

  callProtocol(bool udp_p2p_, bool udp_reflector_, std::int32_t min_layer_, std::int32_t max_layer_);

  static const std::int32_t ID = -1042830667;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CallState: public Object {
 public:
};

class callStatePending final : public CallState {
 public:
  bool is_created_;
  bool is_received_;

  callStatePending();

  callStatePending(bool is_created_, bool is_received_);

  static const std::int32_t ID = 1073048620;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callStateExchangingKeys final : public CallState {
 public:

  callStateExchangingKeys();

  static const std::int32_t ID = -1848149403;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callStateReady final : public CallState {
 public:
  object_ptr<callProtocol> protocol_;
  std::vector<object_ptr<callConnection>> connections_;
  std::string config_;
  std::string encryption_key_;
  std::vector<std::string> emojis_;
  bool allow_p2p_;

  callStateReady();

  callStateReady(object_ptr<callProtocol> &&protocol_, std::vector<object_ptr<callConnection>> &&connections_, std::string const &config_, std::string const &encryption_key_, std::vector<std::string> &&emojis_, bool allow_p2p_);

  static const std::int32_t ID = 1848397705;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callStateHangingUp final : public CallState {
 public:

  callStateHangingUp();

  static const std::int32_t ID = -2133790038;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callStateDiscarded final : public CallState {
 public:
  object_ptr<CallDiscardReason> reason_;
  bool need_rating_;
  bool need_debug_information_;

  callStateDiscarded();

  callStateDiscarded(object_ptr<CallDiscardReason> &&reason_, bool need_rating_, bool need_debug_information_);

  static const std::int32_t ID = -190853167;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callStateError final : public CallState {
 public:
  object_ptr<error> error_;

  callStateError();

  explicit callStateError(object_ptr<error> &&error_);

  static const std::int32_t ID = -975215467;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callbackQueryAnswer final : public Object {
 public:
  std::string text_;
  bool show_alert_;
  std::string url_;

  callbackQueryAnswer();

  callbackQueryAnswer(std::string const &text_, bool show_alert_, std::string const &url_);

  static const std::int32_t ID = 360867933;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CallbackQueryPayload: public Object {
 public:
};

class callbackQueryPayloadData final : public CallbackQueryPayload {
 public:
  std::string data_;

  callbackQueryPayloadData();

  explicit callbackQueryPayloadData(std::string const &data_);

  static const std::int32_t ID = -1977729946;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class callbackQueryPayloadGame final : public CallbackQueryPayload {
 public:
  std::string game_short_name_;

  callbackQueryPayloadGame();

  explicit callbackQueryPayloadGame(std::string const &game_short_name_);

  static const std::int32_t ID = 1303571512;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CanTransferOwnershipResult: public Object {
 public:
};

class canTransferOwnershipResultOk final : public CanTransferOwnershipResult {
 public:

  canTransferOwnershipResultOk();

  static const std::int32_t ID = -89881021;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class canTransferOwnershipResultPasswordNeeded final : public CanTransferOwnershipResult {
 public:

  canTransferOwnershipResultPasswordNeeded();

  static const std::int32_t ID = 1548372703;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class canTransferOwnershipResultPasswordTooFresh final : public CanTransferOwnershipResult {
 public:
  std::int32_t retry_after_;

  canTransferOwnershipResultPasswordTooFresh();

  explicit canTransferOwnershipResultPasswordTooFresh(std::int32_t retry_after_);

  static const std::int32_t ID = 811440913;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class canTransferOwnershipResultSessionTooFresh final : public CanTransferOwnershipResult {
 public:
  std::int32_t retry_after_;

  canTransferOwnershipResultSessionTooFresh();

  explicit canTransferOwnershipResultSessionTooFresh(std::int32_t retry_after_);

  static const std::int32_t ID = 984664289;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chat final : public Object {
 public:
  std::int64_t id_;
  object_ptr<ChatType> type_;
  object_ptr<ChatList> chat_list_;
  std::string title_;
  object_ptr<chatPhoto> photo_;
  object_ptr<chatPermissions> permissions_;
  object_ptr<message> last_message_;
  std::int64_t order_;
  bool is_pinned_;
  bool is_marked_as_unread_;
  bool is_sponsored_;
  bool has_scheduled_messages_;
  bool can_be_deleted_only_for_self_;
  bool can_be_deleted_for_all_users_;
  bool can_be_reported_;
  bool default_disable_notification_;
  std::int32_t unread_count_;
  std::int64_t last_read_inbox_message_id_;
  std::int64_t last_read_outbox_message_id_;
  std::int32_t unread_mention_count_;
  object_ptr<chatNotificationSettings> notification_settings_;
  object_ptr<ChatActionBar> action_bar_;
  std::int64_t pinned_message_id_;
  std::int64_t reply_markup_message_id_;
  object_ptr<draftMessage> draft_message_;
  std::string client_data_;

  chat();

  chat(std::int64_t id_, object_ptr<ChatType> &&type_, object_ptr<ChatList> &&chat_list_, std::string const &title_, object_ptr<chatPhoto> &&photo_, object_ptr<chatPermissions> &&permissions_, object_ptr<message> &&last_message_, std::int64_t order_, bool is_pinned_, bool is_marked_as_unread_, bool is_sponsored_, bool has_scheduled_messages_, bool can_be_deleted_only_for_self_, bool can_be_deleted_for_all_users_, bool can_be_reported_, bool default_disable_notification_, std::int32_t unread_count_, std::int64_t last_read_inbox_message_id_, std::int64_t last_read_outbox_message_id_, std::int32_t unread_mention_count_, object_ptr<chatNotificationSettings> &&notification_settings_, object_ptr<ChatActionBar> &&action_bar_, std::int64_t pinned_message_id_, std::int64_t reply_markup_message_id_, object_ptr<draftMessage> &&draft_message_, std::string const &client_data_);

  static const std::int32_t ID = -861487386;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatAction: public Object {
 public:
};

class chatActionTyping final : public ChatAction {
 public:

  chatActionTyping();

  static const std::int32_t ID = 380122167;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionRecordingVideo final : public ChatAction {
 public:

  chatActionRecordingVideo();

  static const std::int32_t ID = 216553362;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionUploadingVideo final : public ChatAction {
 public:
  std::int32_t progress_;

  chatActionUploadingVideo();

  explicit chatActionUploadingVideo(std::int32_t progress_);

  static const std::int32_t ID = 1234185270;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionRecordingVoiceNote final : public ChatAction {
 public:

  chatActionRecordingVoiceNote();

  static const std::int32_t ID = -808850058;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionUploadingVoiceNote final : public ChatAction {
 public:
  std::int32_t progress_;

  chatActionUploadingVoiceNote();

  explicit chatActionUploadingVoiceNote(std::int32_t progress_);

  static const std::int32_t ID = -613643666;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionUploadingPhoto final : public ChatAction {
 public:
  std::int32_t progress_;

  chatActionUploadingPhoto();

  explicit chatActionUploadingPhoto(std::int32_t progress_);

  static const std::int32_t ID = 654240583;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionUploadingDocument final : public ChatAction {
 public:
  std::int32_t progress_;

  chatActionUploadingDocument();

  explicit chatActionUploadingDocument(std::int32_t progress_);

  static const std::int32_t ID = 167884362;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionChoosingLocation final : public ChatAction {
 public:

  chatActionChoosingLocation();

  static const std::int32_t ID = -2017893596;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionChoosingContact final : public ChatAction {
 public:

  chatActionChoosingContact();

  static const std::int32_t ID = -1222507496;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionStartPlayingGame final : public ChatAction {
 public:

  chatActionStartPlayingGame();

  static const std::int32_t ID = -865884164;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionRecordingVideoNote final : public ChatAction {
 public:

  chatActionRecordingVideoNote();

  static const std::int32_t ID = 16523393;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionUploadingVideoNote final : public ChatAction {
 public:
  std::int32_t progress_;

  chatActionUploadingVideoNote();

  explicit chatActionUploadingVideoNote(std::int32_t progress_);

  static const std::int32_t ID = 1172364918;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionCancel final : public ChatAction {
 public:

  chatActionCancel();

  static const std::int32_t ID = 1160523958;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatActionBar: public Object {
 public:
};

class chatActionBarReportSpam final : public ChatActionBar {
 public:

  chatActionBarReportSpam();

  static const std::int32_t ID = -1603417249;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionBarReportUnrelatedLocation final : public ChatActionBar {
 public:

  chatActionBarReportUnrelatedLocation();

  static const std::int32_t ID = 758175489;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionBarReportAddBlock final : public ChatActionBar {
 public:

  chatActionBarReportAddBlock();

  static const std::int32_t ID = -87894249;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionBarAddContact final : public ChatActionBar {
 public:

  chatActionBarAddContact();

  static const std::int32_t ID = -733325295;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatActionBarSharePhoneNumber final : public ChatActionBar {
 public:

  chatActionBarSharePhoneNumber();

  static const std::int32_t ID = 35188697;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatAdministrator final : public Object {
 public:
  std::int32_t user_id_;
  std::string custom_title_;
  bool is_owner_;

  chatAdministrator();

  chatAdministrator(std::int32_t user_id_, std::string const &custom_title_, bool is_owner_);

  static const std::int32_t ID = 487220942;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatAdministrators final : public Object {
 public:
  std::vector<object_ptr<chatAdministrator>> administrators_;

  chatAdministrators();

  explicit chatAdministrators(std::vector<object_ptr<chatAdministrator>> &&administrators_);

  static const std::int32_t ID = -2126186435;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEvent final : public Object {
 public:
  std::int64_t id_;
  std::int32_t date_;
  std::int32_t user_id_;
  object_ptr<ChatEventAction> action_;

  chatEvent();

  chatEvent(std::int64_t id_, std::int32_t date_, std::int32_t user_id_, object_ptr<ChatEventAction> &&action_);

  static const std::int32_t ID = -609912404;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatEventAction: public Object {
 public:
};

class chatEventMessageEdited final : public ChatEventAction {
 public:
  object_ptr<message> old_message_;
  object_ptr<message> new_message_;

  chatEventMessageEdited();

  chatEventMessageEdited(object_ptr<message> &&old_message_, object_ptr<message> &&new_message_);

  static const std::int32_t ID = -430967304;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMessageDeleted final : public ChatEventAction {
 public:
  object_ptr<message> message_;

  chatEventMessageDeleted();

  explicit chatEventMessageDeleted(object_ptr<message> &&message_);

  static const std::int32_t ID = -892974601;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventPollStopped final : public ChatEventAction {
 public:
  object_ptr<message> message_;

  chatEventPollStopped();

  explicit chatEventPollStopped(object_ptr<message> &&message_);

  static const std::int32_t ID = 2009893861;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMessagePinned final : public ChatEventAction {
 public:
  object_ptr<message> message_;

  chatEventMessagePinned();

  explicit chatEventMessagePinned(object_ptr<message> &&message_);

  static const std::int32_t ID = 438742298;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMessageUnpinned final : public ChatEventAction {
 public:

  chatEventMessageUnpinned();

  static const std::int32_t ID = 2002594849;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMemberJoined final : public ChatEventAction {
 public:

  chatEventMemberJoined();

  static const std::int32_t ID = -235468508;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMemberLeft final : public ChatEventAction {
 public:

  chatEventMemberLeft();

  static const std::int32_t ID = -948420593;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMemberInvited final : public ChatEventAction {
 public:
  std::int32_t user_id_;
  object_ptr<ChatMemberStatus> status_;

  chatEventMemberInvited();

  chatEventMemberInvited(std::int32_t user_id_, object_ptr<ChatMemberStatus> &&status_);

  static const std::int32_t ID = -2093688706;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMemberPromoted final : public ChatEventAction {
 public:
  std::int32_t user_id_;
  object_ptr<ChatMemberStatus> old_status_;
  object_ptr<ChatMemberStatus> new_status_;

  chatEventMemberPromoted();

  chatEventMemberPromoted(std::int32_t user_id_, object_ptr<ChatMemberStatus> &&old_status_, object_ptr<ChatMemberStatus> &&new_status_);

  static const std::int32_t ID = 1887176186;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventMemberRestricted final : public ChatEventAction {
 public:
  std::int32_t user_id_;
  object_ptr<ChatMemberStatus> old_status_;
  object_ptr<ChatMemberStatus> new_status_;

  chatEventMemberRestricted();

  chatEventMemberRestricted(std::int32_t user_id_, object_ptr<ChatMemberStatus> &&old_status_, object_ptr<ChatMemberStatus> &&new_status_);

  static const std::int32_t ID = 584946294;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventTitleChanged final : public ChatEventAction {
 public:
  std::string old_title_;
  std::string new_title_;

  chatEventTitleChanged();

  chatEventTitleChanged(std::string const &old_title_, std::string const &new_title_);

  static const std::int32_t ID = 1134103250;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventPermissionsChanged final : public ChatEventAction {
 public:
  object_ptr<chatPermissions> old_permissions_;
  object_ptr<chatPermissions> new_permissions_;

  chatEventPermissionsChanged();

  chatEventPermissionsChanged(object_ptr<chatPermissions> &&old_permissions_, object_ptr<chatPermissions> &&new_permissions_);

  static const std::int32_t ID = -1311557720;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventDescriptionChanged final : public ChatEventAction {
 public:
  std::string old_description_;
  std::string new_description_;

  chatEventDescriptionChanged();

  chatEventDescriptionChanged(std::string const &old_description_, std::string const &new_description_);

  static const std::int32_t ID = 39112478;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventUsernameChanged final : public ChatEventAction {
 public:
  std::string old_username_;
  std::string new_username_;

  chatEventUsernameChanged();

  chatEventUsernameChanged(std::string const &old_username_, std::string const &new_username_);

  static const std::int32_t ID = 1728558443;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventPhotoChanged final : public ChatEventAction {
 public:
  object_ptr<photo> old_photo_;
  object_ptr<photo> new_photo_;

  chatEventPhotoChanged();

  chatEventPhotoChanged(object_ptr<photo> &&old_photo_, object_ptr<photo> &&new_photo_);

  static const std::int32_t ID = 1037662734;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventInvitesToggled final : public ChatEventAction {
 public:
  bool can_invite_users_;

  chatEventInvitesToggled();

  explicit chatEventInvitesToggled(bool can_invite_users_);

  static const std::int32_t ID = -62548373;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventLinkedChatChanged final : public ChatEventAction {
 public:
  std::int64_t old_linked_chat_id_;
  std::int64_t new_linked_chat_id_;

  chatEventLinkedChatChanged();

  chatEventLinkedChatChanged(std::int64_t old_linked_chat_id_, std::int64_t new_linked_chat_id_);

  static const std::int32_t ID = 1797419439;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventSlowModeDelayChanged final : public ChatEventAction {
 public:
  std::int32_t old_slow_mode_delay_;
  std::int32_t new_slow_mode_delay_;

  chatEventSlowModeDelayChanged();

  chatEventSlowModeDelayChanged(std::int32_t old_slow_mode_delay_, std::int32_t new_slow_mode_delay_);

  static const std::int32_t ID = -1653195765;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventSignMessagesToggled final : public ChatEventAction {
 public:
  bool sign_messages_;

  chatEventSignMessagesToggled();

  explicit chatEventSignMessagesToggled(bool sign_messages_);

  static const std::int32_t ID = -1313265634;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventStickerSetChanged final : public ChatEventAction {
 public:
  std::int64_t old_sticker_set_id_;
  std::int64_t new_sticker_set_id_;

  chatEventStickerSetChanged();

  chatEventStickerSetChanged(std::int64_t old_sticker_set_id_, std::int64_t new_sticker_set_id_);

  static const std::int32_t ID = -1243130481;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventLocationChanged final : public ChatEventAction {
 public:
  object_ptr<chatLocation> old_location_;
  object_ptr<chatLocation> new_location_;

  chatEventLocationChanged();

  chatEventLocationChanged(object_ptr<chatLocation> &&old_location_, object_ptr<chatLocation> &&new_location_);

  static const std::int32_t ID = -405930674;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventIsAllHistoryAvailableToggled final : public ChatEventAction {
 public:
  bool is_all_history_available_;

  chatEventIsAllHistoryAvailableToggled();

  explicit chatEventIsAllHistoryAvailableToggled(bool is_all_history_available_);

  static const std::int32_t ID = -1599063019;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEventLogFilters final : public Object {
 public:
  bool message_edits_;
  bool message_deletions_;
  bool message_pins_;
  bool member_joins_;
  bool member_leaves_;
  bool member_invites_;
  bool member_promotions_;
  bool member_restrictions_;
  bool info_changes_;
  bool setting_changes_;

  chatEventLogFilters();

  chatEventLogFilters(bool message_edits_, bool message_deletions_, bool message_pins_, bool member_joins_, bool member_leaves_, bool member_invites_, bool member_promotions_, bool member_restrictions_, bool info_changes_, bool setting_changes_);

  static const std::int32_t ID = 941939684;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatEvents final : public Object {
 public:
  std::vector<object_ptr<chatEvent>> events_;

  chatEvents();

  explicit chatEvents(std::vector<object_ptr<chatEvent>> &&events_);

  static const std::int32_t ID = -585329664;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatInviteLink final : public Object {
 public:
  std::string invite_link_;

  chatInviteLink();

  explicit chatInviteLink(std::string const &invite_link_);

  static const std::int32_t ID = -882072492;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatInviteLinkInfo final : public Object {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatType> type_;
  std::string title_;
  object_ptr<chatPhoto> photo_;
  std::int32_t member_count_;
  std::vector<std::int32_t> member_user_ids_;
  bool is_public_;

  chatInviteLinkInfo();

  chatInviteLinkInfo(std::int64_t chat_id_, object_ptr<ChatType> &&type_, std::string const &title_, object_ptr<chatPhoto> &&photo_, std::int32_t member_count_, std::vector<std::int32_t> &&member_user_ids_, bool is_public_);

  static const std::int32_t ID = -323394424;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatList: public Object {
 public:
};

class chatListMain final : public ChatList {
 public:

  chatListMain();

  static const std::int32_t ID = -400991316;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatListArchive final : public ChatList {
 public:

  chatListArchive();

  static const std::int32_t ID = 362770115;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatLocation final : public Object {
 public:
  object_ptr<location> location_;
  std::string address_;

  chatLocation();

  chatLocation(object_ptr<location> &&location_, std::string const &address_);

  static const std::int32_t ID = -1566863583;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMember final : public Object {
 public:
  std::int32_t user_id_;
  std::int32_t inviter_user_id_;
  std::int32_t joined_chat_date_;
  object_ptr<ChatMemberStatus> status_;
  object_ptr<botInfo> bot_info_;

  chatMember();

  chatMember(std::int32_t user_id_, std::int32_t inviter_user_id_, std::int32_t joined_chat_date_, object_ptr<ChatMemberStatus> &&status_, object_ptr<botInfo> &&bot_info_);

  static const std::int32_t ID = -806137076;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatMemberStatus: public Object {
 public:
};

class chatMemberStatusCreator final : public ChatMemberStatus {
 public:
  std::string custom_title_;
  bool is_member_;

  chatMemberStatusCreator();

  chatMemberStatusCreator(std::string const &custom_title_, bool is_member_);

  static const std::int32_t ID = 2038475849;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMemberStatusAdministrator final : public ChatMemberStatus {
 public:
  std::string custom_title_;
  bool can_be_edited_;
  bool can_change_info_;
  bool can_post_messages_;
  bool can_edit_messages_;
  bool can_delete_messages_;
  bool can_invite_users_;
  bool can_restrict_members_;
  bool can_pin_messages_;
  bool can_promote_members_;

  chatMemberStatusAdministrator();

  chatMemberStatusAdministrator(std::string const &custom_title_, bool can_be_edited_, bool can_change_info_, bool can_post_messages_, bool can_edit_messages_, bool can_delete_messages_, bool can_invite_users_, bool can_restrict_members_, bool can_pin_messages_, bool can_promote_members_);

  static const std::int32_t ID = 1800612058;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMemberStatusMember final : public ChatMemberStatus {
 public:

  chatMemberStatusMember();

  static const std::int32_t ID = 844723285;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMemberStatusRestricted final : public ChatMemberStatus {
 public:
  bool is_member_;
  std::int32_t restricted_until_date_;
  object_ptr<chatPermissions> permissions_;

  chatMemberStatusRestricted();

  chatMemberStatusRestricted(bool is_member_, std::int32_t restricted_until_date_, object_ptr<chatPermissions> &&permissions_);

  static const std::int32_t ID = 1661432998;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMemberStatusLeft final : public ChatMemberStatus {
 public:

  chatMemberStatusLeft();

  static const std::int32_t ID = -5815259;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMemberStatusBanned final : public ChatMemberStatus {
 public:
  std::int32_t banned_until_date_;

  chatMemberStatusBanned();

  explicit chatMemberStatusBanned(std::int32_t banned_until_date_);

  static const std::int32_t ID = -1653518666;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembers final : public Object {
 public:
  std::int32_t total_count_;
  std::vector<object_ptr<chatMember>> members_;

  chatMembers();

  chatMembers(std::int32_t total_count_, std::vector<object_ptr<chatMember>> &&members_);

  static const std::int32_t ID = -497558622;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatMembersFilter: public Object {
 public:
};

class chatMembersFilterContacts final : public ChatMembersFilter {
 public:

  chatMembersFilterContacts();

  static const std::int32_t ID = 1774485671;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembersFilterAdministrators final : public ChatMembersFilter {
 public:

  chatMembersFilterAdministrators();

  static const std::int32_t ID = -1266893796;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembersFilterMembers final : public ChatMembersFilter {
 public:

  chatMembersFilterMembers();

  static const std::int32_t ID = 670504342;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembersFilterRestricted final : public ChatMembersFilter {
 public:

  chatMembersFilterRestricted();

  static const std::int32_t ID = 1256282813;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembersFilterBanned final : public ChatMembersFilter {
 public:

  chatMembersFilterBanned();

  static const std::int32_t ID = -1863102648;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatMembersFilterBots final : public ChatMembersFilter {
 public:

  chatMembersFilterBots();

  static const std::int32_t ID = -1422567288;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatNearby final : public Object {
 public:
  std::int64_t chat_id_;
  std::int32_t distance_;

  chatNearby();

  chatNearby(std::int64_t chat_id_, std::int32_t distance_);

  static const std::int32_t ID = 48120405;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatNotificationSettings final : public Object {
 public:
  bool use_default_mute_for_;
  std::int32_t mute_for_;
  bool use_default_sound_;
  std::string sound_;
  bool use_default_show_preview_;
  bool show_preview_;
  bool use_default_disable_pinned_message_notifications_;
  bool disable_pinned_message_notifications_;
  bool use_default_disable_mention_notifications_;
  bool disable_mention_notifications_;

  chatNotificationSettings();

  chatNotificationSettings(bool use_default_mute_for_, std::int32_t mute_for_, bool use_default_sound_, std::string const &sound_, bool use_default_show_preview_, bool show_preview_, bool use_default_disable_pinned_message_notifications_, bool disable_pinned_message_notifications_, bool use_default_disable_mention_notifications_, bool disable_mention_notifications_);

  static const std::int32_t ID = 1503183218;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatPermissions final : public Object {
 public:
  bool can_send_messages_;
  bool can_send_media_messages_;
  bool can_send_polls_;
  bool can_send_other_messages_;
  bool can_add_web_page_previews_;
  bool can_change_info_;
  bool can_invite_users_;
  bool can_pin_messages_;

  chatPermissions();

  chatPermissions(bool can_send_messages_, bool can_send_media_messages_, bool can_send_polls_, bool can_send_other_messages_, bool can_add_web_page_previews_, bool can_change_info_, bool can_invite_users_, bool can_pin_messages_);

  static const std::int32_t ID = 1584650463;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatPhoto final : public Object {
 public:
  object_ptr<file> small_;
  object_ptr<file> big_;

  chatPhoto();

  chatPhoto(object_ptr<file> &&small_, object_ptr<file> &&big_);

  static const std::int32_t ID = -217062456;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatReportReason: public Object {
 public:
};

class chatReportReasonSpam final : public ChatReportReason {
 public:

  chatReportReasonSpam();

  static const std::int32_t ID = -510848863;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonViolence final : public ChatReportReason {
 public:

  chatReportReasonViolence();

  static const std::int32_t ID = -1330235395;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonPornography final : public ChatReportReason {
 public:

  chatReportReasonPornography();

  static const std::int32_t ID = 722614385;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonChildAbuse final : public ChatReportReason {
 public:

  chatReportReasonChildAbuse();

  static const std::int32_t ID = -1070686531;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonCopyright final : public ChatReportReason {
 public:

  chatReportReasonCopyright();

  static const std::int32_t ID = 986898080;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonUnrelatedLocation final : public ChatReportReason {
 public:

  chatReportReasonUnrelatedLocation();

  static const std::int32_t ID = 2632403;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatReportReasonCustom final : public ChatReportReason {
 public:
  std::string text_;

  chatReportReasonCustom();

  explicit chatReportReasonCustom(std::string const &text_);

  static const std::int32_t ID = 544575454;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ChatType: public Object {
 public:
};

class chatTypePrivate final : public ChatType {
 public:
  std::int32_t user_id_;

  chatTypePrivate();

  explicit chatTypePrivate(std::int32_t user_id_);

  static const std::int32_t ID = 1700720838;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatTypeBasicGroup final : public ChatType {
 public:
  std::int32_t basic_group_id_;

  chatTypeBasicGroup();

  explicit chatTypeBasicGroup(std::int32_t basic_group_id_);

  static const std::int32_t ID = 21815278;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatTypeSupergroup final : public ChatType {
 public:
  std::int32_t supergroup_id_;
  bool is_channel_;

  chatTypeSupergroup();

  chatTypeSupergroup(std::int32_t supergroup_id_, bool is_channel_);

  static const std::int32_t ID = 955152366;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatTypeSecret final : public ChatType {
 public:
  std::int32_t secret_chat_id_;
  std::int32_t user_id_;

  chatTypeSecret();

  chatTypeSecret(std::int32_t secret_chat_id_, std::int32_t user_id_);

  static const std::int32_t ID = 136722563;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chats final : public Object {
 public:
  std::vector<std::int64_t> chat_ids_;

  chats();

  explicit chats(std::vector<std::int64_t> &&chat_ids_);

  static const std::int32_t ID = -1687756019;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class chatsNearby final : public Object {
 public:
  std::vector<object_ptr<chatNearby>> users_nearby_;
  std::vector<object_ptr<chatNearby>> supergroups_nearby_;

  chatsNearby();

  chatsNearby(std::vector<object_ptr<chatNearby>> &&users_nearby_, std::vector<object_ptr<chatNearby>> &&supergroups_nearby_);

  static const std::int32_t ID = 187746081;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class CheckChatUsernameResult: public Object {
 public:
};

class checkChatUsernameResultOk final : public CheckChatUsernameResult {
 public:

  checkChatUsernameResultOk();

  static const std::int32_t ID = -1498956964;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatUsernameResultUsernameInvalid final : public CheckChatUsernameResult {
 public:

  checkChatUsernameResultUsernameInvalid();

  static const std::int32_t ID = -636979370;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatUsernameResultUsernameOccupied final : public CheckChatUsernameResult {
 public:

  checkChatUsernameResultUsernameOccupied();

  static const std::int32_t ID = 1320892201;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatUsernameResultPublicChatsTooMuch final : public CheckChatUsernameResult {
 public:

  checkChatUsernameResultPublicChatsTooMuch();

  static const std::int32_t ID = 858247741;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatUsernameResultPublicGroupsUnavailable final : public CheckChatUsernameResult {
 public:

  checkChatUsernameResultPublicGroupsUnavailable();

  static const std::int32_t ID = -51833641;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectedWebsite final : public Object {
 public:
  std::int64_t id_;
  std::string domain_name_;
  std::int32_t bot_user_id_;
  std::string browser_;
  std::string platform_;
  std::int32_t log_in_date_;
  std::int32_t last_active_date_;
  std::string ip_;
  std::string location_;

  connectedWebsite();

  connectedWebsite(std::int64_t id_, std::string const &domain_name_, std::int32_t bot_user_id_, std::string const &browser_, std::string const &platform_, std::int32_t log_in_date_, std::int32_t last_active_date_, std::string const &ip_, std::string const &location_);

  static const std::int32_t ID = -1538986855;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectedWebsites final : public Object {
 public:
  std::vector<object_ptr<connectedWebsite>> websites_;

  connectedWebsites();

  explicit connectedWebsites(std::vector<object_ptr<connectedWebsite>> &&websites_);

  static const std::int32_t ID = -1727949694;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ConnectionState: public Object {
 public:
};

class connectionStateWaitingForNetwork final : public ConnectionState {
 public:

  connectionStateWaitingForNetwork();

  static const std::int32_t ID = 1695405912;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectionStateConnectingToProxy final : public ConnectionState {
 public:

  connectionStateConnectingToProxy();

  static const std::int32_t ID = -93187239;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectionStateConnecting final : public ConnectionState {
 public:

  connectionStateConnecting();

  static const std::int32_t ID = -1298400670;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectionStateUpdating final : public ConnectionState {
 public:

  connectionStateUpdating();

  static const std::int32_t ID = -188104009;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class connectionStateReady final : public ConnectionState {
 public:

  connectionStateReady();

  static const std::int32_t ID = 48608492;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class contact final : public Object {
 public:
  std::string phone_number_;
  std::string first_name_;
  std::string last_name_;
  std::string vcard_;
  std::int32_t user_id_;

  contact();

  contact(std::string const &phone_number_, std::string const &first_name_, std::string const &last_name_, std::string const &vcard_, std::int32_t user_id_);

  static const std::int32_t ID = -1483002540;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class count final : public Object {
 public:
  std::int32_t count_;

  count();

  explicit count(std::int32_t count_);

  static const std::int32_t ID = 1295577348;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class customRequestResult final : public Object {
 public:
  std::string result_;

  customRequestResult();

  explicit customRequestResult(std::string const &result_);

  static const std::int32_t ID = -2009960452;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class databaseStatistics final : public Object {
 public:
  std::string statistics_;

  databaseStatistics();

  explicit databaseStatistics(std::string const &statistics_);

  static const std::int32_t ID = -1123912880;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class date final : public Object {
 public:
  std::int32_t day_;
  std::int32_t month_;
  std::int32_t year_;

  date();

  date(std::int32_t day_, std::int32_t month_, std::int32_t year_);

  static const std::int32_t ID = -277956960;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class datedFile final : public Object {
 public:
  object_ptr<file> file_;
  std::int32_t date_;

  datedFile();

  datedFile(object_ptr<file> &&file_, std::int32_t date_);

  static const std::int32_t ID = -1840795491;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deepLinkInfo final : public Object {
 public:
  object_ptr<formattedText> text_;
  bool need_update_application_;

  deepLinkInfo();

  deepLinkInfo(object_ptr<formattedText> &&text_, bool need_update_application_);

  static const std::int32_t ID = 1864081662;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class DeviceToken: public Object {
 public:
};

class deviceTokenFirebaseCloudMessaging final : public DeviceToken {
 public:
  std::string token_;
  bool encrypt_;

  deviceTokenFirebaseCloudMessaging();

  deviceTokenFirebaseCloudMessaging(std::string const &token_, bool encrypt_);

  static const std::int32_t ID = -797881849;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenApplePush final : public DeviceToken {
 public:
  std::string device_token_;
  bool is_app_sandbox_;

  deviceTokenApplePush();

  deviceTokenApplePush(std::string const &device_token_, bool is_app_sandbox_);

  static const std::int32_t ID = 387541955;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenApplePushVoIP final : public DeviceToken {
 public:
  std::string device_token_;
  bool is_app_sandbox_;
  bool encrypt_;

  deviceTokenApplePushVoIP();

  deviceTokenApplePushVoIP(std::string const &device_token_, bool is_app_sandbox_, bool encrypt_);

  static const std::int32_t ID = 804275689;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenWindowsPush final : public DeviceToken {
 public:
  std::string access_token_;

  deviceTokenWindowsPush();

  explicit deviceTokenWindowsPush(std::string const &access_token_);

  static const std::int32_t ID = -1410514289;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenMicrosoftPush final : public DeviceToken {
 public:
  std::string channel_uri_;

  deviceTokenMicrosoftPush();

  explicit deviceTokenMicrosoftPush(std::string const &channel_uri_);

  static const std::int32_t ID = 1224269900;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenMicrosoftPushVoIP final : public DeviceToken {
 public:
  std::string channel_uri_;

  deviceTokenMicrosoftPushVoIP();

  explicit deviceTokenMicrosoftPushVoIP(std::string const &channel_uri_);

  static const std::int32_t ID = -785603759;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenWebPush final : public DeviceToken {
 public:
  std::string endpoint_;
  std::string p256dh_base64url_;
  std::string auth_base64url_;

  deviceTokenWebPush();

  deviceTokenWebPush(std::string const &endpoint_, std::string const &p256dh_base64url_, std::string const &auth_base64url_);

  static const std::int32_t ID = -1694507273;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenSimplePush final : public DeviceToken {
 public:
  std::string endpoint_;

  deviceTokenSimplePush();

  explicit deviceTokenSimplePush(std::string const &endpoint_);

  static const std::int32_t ID = 49584736;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenUbuntuPush final : public DeviceToken {
 public:
  std::string token_;

  deviceTokenUbuntuPush();

  explicit deviceTokenUbuntuPush(std::string const &token_);

  static const std::int32_t ID = 1782320422;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenBlackBerryPush final : public DeviceToken {
 public:
  std::string token_;

  deviceTokenBlackBerryPush();

  explicit deviceTokenBlackBerryPush(std::string const &token_);

  static const std::int32_t ID = 1559167234;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deviceTokenTizenPush final : public DeviceToken {
 public:
  std::string reg_id_;

  deviceTokenTizenPush();

  explicit deviceTokenTizenPush(std::string const &reg_id_);

  static const std::int32_t ID = -1359947213;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class document final : public Object {
 public:
  std::string file_name_;
  std::string mime_type_;
  object_ptr<minithumbnail> minithumbnail_;
  object_ptr<photoSize> thumbnail_;
  object_ptr<file> document_;

  document();

  document(std::string const &file_name_, std::string const &mime_type_, object_ptr<minithumbnail> &&minithumbnail_, object_ptr<photoSize> &&thumbnail_, object_ptr<file> &&document_);

  static const std::int32_t ID = 21881988;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class draftMessage final : public Object {
 public:
  std::int64_t reply_to_message_id_;
  object_ptr<InputMessageContent> input_message_text_;

  draftMessage();

  draftMessage(std::int64_t reply_to_message_id_, object_ptr<InputMessageContent> &&input_message_text_);

  static const std::int32_t ID = 1902914742;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class emailAddressAuthenticationCodeInfo final : public Object {
 public:
  std::string email_address_pattern_;
  std::int32_t length_;

  emailAddressAuthenticationCodeInfo();

  emailAddressAuthenticationCodeInfo(std::string const &email_address_pattern_, std::int32_t length_);

  static const std::int32_t ID = 1151066659;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class emojis final : public Object {
 public:
  std::vector<std::string> emojis_;

  emojis();

  explicit emojis(std::vector<std::string> &&emojis_);

  static const std::int32_t ID = 950339552;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class encryptedCredentials final : public Object {
 public:
  std::string data_;
  std::string hash_;
  std::string secret_;

  encryptedCredentials();

  encryptedCredentials(std::string const &data_, std::string const &hash_, std::string const &secret_);

  static const std::int32_t ID = 1331106766;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class encryptedPassportElement final : public Object {
 public:
  object_ptr<PassportElementType> type_;
  std::string data_;
  object_ptr<datedFile> front_side_;
  object_ptr<datedFile> reverse_side_;
  object_ptr<datedFile> selfie_;
  std::vector<object_ptr<datedFile>> translation_;
  std::vector<object_ptr<datedFile>> files_;
  std::string value_;
  std::string hash_;

  encryptedPassportElement();

  encryptedPassportElement(object_ptr<PassportElementType> &&type_, std::string const &data_, object_ptr<datedFile> &&front_side_, object_ptr<datedFile> &&reverse_side_, object_ptr<datedFile> &&selfie_, std::vector<object_ptr<datedFile>> &&translation_, std::vector<object_ptr<datedFile>> &&files_, std::string const &value_, std::string const &hash_);

  static const std::int32_t ID = 2002386193;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class error final : public Object {
 public:
  std::int32_t code_;
  std::string message_;

  error();

  error(std::int32_t code_, std::string const &message_);

  static const std::int32_t ID = -1679978726;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class file final : public Object {
 public:
  std::int32_t id_;
  std::int32_t size_;
  std::int32_t expected_size_;
  object_ptr<localFile> local_;
  object_ptr<remoteFile> remote_;

  file();

  file(std::int32_t id_, std::int32_t size_, std::int32_t expected_size_, object_ptr<localFile> &&local_, object_ptr<remoteFile> &&remote_);

  static const std::int32_t ID = 766337656;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class filePart final : public Object {
 public:
  std::string data_;

  filePart();

  explicit filePart(std::string const &data_);

  static const std::int32_t ID = 911821878;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class FileType: public Object {
 public:
};

class fileTypeNone final : public FileType {
 public:

  fileTypeNone();

  static const std::int32_t ID = 2003009189;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeAnimation final : public FileType {
 public:

  fileTypeAnimation();

  static const std::int32_t ID = -290816582;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeAudio final : public FileType {
 public:

  fileTypeAudio();

  static const std::int32_t ID = -709112160;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeDocument final : public FileType {
 public:

  fileTypeDocument();

  static const std::int32_t ID = -564722929;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypePhoto final : public FileType {
 public:

  fileTypePhoto();

  static const std::int32_t ID = -1718914651;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeProfilePhoto final : public FileType {
 public:

  fileTypeProfilePhoto();

  static const std::int32_t ID = 1795089315;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeSecret final : public FileType {
 public:

  fileTypeSecret();

  static const std::int32_t ID = -1871899401;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeSecretThumbnail final : public FileType {
 public:

  fileTypeSecretThumbnail();

  static const std::int32_t ID = -1401326026;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeSecure final : public FileType {
 public:

  fileTypeSecure();

  static const std::int32_t ID = -1419133146;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeSticker final : public FileType {
 public:

  fileTypeSticker();

  static const std::int32_t ID = 475233385;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeThumbnail final : public FileType {
 public:

  fileTypeThumbnail();

  static const std::int32_t ID = -12443298;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeUnknown final : public FileType {
 public:

  fileTypeUnknown();

  static const std::int32_t ID = -2011566768;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeVideo final : public FileType {
 public:

  fileTypeVideo();

  static const std::int32_t ID = 1430816539;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeVideoNote final : public FileType {
 public:

  fileTypeVideoNote();

  static const std::int32_t ID = -518412385;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeVoiceNote final : public FileType {
 public:

  fileTypeVoiceNote();

  static const std::int32_t ID = -588681661;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class fileTypeWallpaper final : public FileType {
 public:

  fileTypeWallpaper();

  static const std::int32_t ID = 1854930076;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class formattedText final : public Object {
 public:
  std::string text_;
  std::vector<object_ptr<textEntity>> entities_;

  formattedText();

  formattedText(std::string const &text_, std::vector<object_ptr<textEntity>> &&entities_);

  static const std::int32_t ID = -252624564;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class foundMessages final : public Object {
 public:
  std::vector<object_ptr<message>> messages_;
  std::int64_t next_from_search_id_;

  foundMessages();

  foundMessages(std::vector<object_ptr<message>> &&messages_, std::int64_t next_from_search_id_);

  static const std::int32_t ID = 2135623881;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class game final : public Object {
 public:
  std::int64_t id_;
  std::string short_name_;
  std::string title_;
  object_ptr<formattedText> text_;
  std::string description_;
  object_ptr<photo> photo_;
  object_ptr<animation> animation_;

  game();

  game(std::int64_t id_, std::string const &short_name_, std::string const &title_, object_ptr<formattedText> &&text_, std::string const &description_, object_ptr<photo> &&photo_, object_ptr<animation> &&animation_);

  static const std::int32_t ID = -1565597752;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class gameHighScore final : public Object {
 public:
  std::int32_t position_;
  std::int32_t user_id_;
  std::int32_t score_;

  gameHighScore();

  gameHighScore(std::int32_t position_, std::int32_t user_id_, std::int32_t score_);

  static const std::int32_t ID = -30778358;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class gameHighScores final : public Object {
 public:
  std::vector<object_ptr<gameHighScore>> scores_;

  gameHighScores();

  explicit gameHighScores(std::vector<object_ptr<gameHighScore>> &&scores_);

  static const std::int32_t ID = -725770727;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class hashtags final : public Object {
 public:
  std::vector<std::string> hashtags_;

  hashtags();

  explicit hashtags(std::vector<std::string> &&hashtags_);

  static const std::int32_t ID = 676798885;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class httpUrl final : public Object {
 public:
  std::string url_;

  httpUrl();

  explicit httpUrl(std::string const &url_);

  static const std::int32_t ID = -2018019930;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class identityDocument final : public Object {
 public:
  std::string number_;
  object_ptr<date> expiry_date_;
  object_ptr<datedFile> front_side_;
  object_ptr<datedFile> reverse_side_;
  object_ptr<datedFile> selfie_;
  std::vector<object_ptr<datedFile>> translation_;

  identityDocument();

  identityDocument(std::string const &number_, object_ptr<date> &&expiry_date_, object_ptr<datedFile> &&front_side_, object_ptr<datedFile> &&reverse_side_, object_ptr<datedFile> &&selfie_, std::vector<object_ptr<datedFile>> &&translation_);

  static const std::int32_t ID = 445952972;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class importedContacts final : public Object {
 public:
  std::vector<std::int32_t> user_ids_;
  std::vector<std::int32_t> importer_count_;

  importedContacts();

  importedContacts(std::vector<std::int32_t> &&user_ids_, std::vector<std::int32_t> &&importer_count_);

  static const std::int32_t ID = -741685354;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButton final : public Object {
 public:
  std::string text_;
  object_ptr<InlineKeyboardButtonType> type_;

  inlineKeyboardButton();

  inlineKeyboardButton(std::string const &text_, object_ptr<InlineKeyboardButtonType> &&type_);

  static const std::int32_t ID = -372105704;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InlineKeyboardButtonType: public Object {
 public:
};

class inlineKeyboardButtonTypeUrl final : public InlineKeyboardButtonType {
 public:
  std::string url_;

  inlineKeyboardButtonTypeUrl();

  explicit inlineKeyboardButtonTypeUrl(std::string const &url_);

  static const std::int32_t ID = 1130741420;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButtonTypeLoginUrl final : public InlineKeyboardButtonType {
 public:
  std::string url_;
  std::int32_t id_;
  std::string forward_text_;

  inlineKeyboardButtonTypeLoginUrl();

  inlineKeyboardButtonTypeLoginUrl(std::string const &url_, std::int32_t id_, std::string const &forward_text_);

  static const std::int32_t ID = 281435539;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButtonTypeCallback final : public InlineKeyboardButtonType {
 public:
  std::string data_;

  inlineKeyboardButtonTypeCallback();

  explicit inlineKeyboardButtonTypeCallback(std::string const &data_);

  static const std::int32_t ID = -1127515139;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButtonTypeCallbackGame final : public InlineKeyboardButtonType {
 public:

  inlineKeyboardButtonTypeCallbackGame();

  static const std::int32_t ID = -383429528;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButtonTypeSwitchInline final : public InlineKeyboardButtonType {
 public:
  std::string query_;
  bool in_current_chat_;

  inlineKeyboardButtonTypeSwitchInline();

  inlineKeyboardButtonTypeSwitchInline(std::string const &query_, bool in_current_chat_);

  static const std::int32_t ID = -2035563307;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineKeyboardButtonTypeBuy final : public InlineKeyboardButtonType {
 public:

  inlineKeyboardButtonTypeBuy();

  static const std::int32_t ID = 1360739440;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InlineQueryResult: public Object {
 public:
};

class inlineQueryResultArticle final : public InlineQueryResult {
 public:
  std::string id_;
  std::string url_;
  bool hide_url_;
  std::string title_;
  std::string description_;
  object_ptr<photoSize> thumbnail_;

  inlineQueryResultArticle();

  inlineQueryResultArticle(std::string const &id_, std::string const &url_, bool hide_url_, std::string const &title_, std::string const &description_, object_ptr<photoSize> &&thumbnail_);

  static const std::int32_t ID = -518366710;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultContact final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<contact> contact_;
  object_ptr<photoSize> thumbnail_;

  inlineQueryResultContact();

  inlineQueryResultContact(std::string const &id_, object_ptr<contact> &&contact_, object_ptr<photoSize> &&thumbnail_);

  static const std::int32_t ID = 410081985;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultLocation final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<location> location_;
  std::string title_;
  object_ptr<photoSize> thumbnail_;

  inlineQueryResultLocation();

  inlineQueryResultLocation(std::string const &id_, object_ptr<location> &&location_, std::string const &title_, object_ptr<photoSize> &&thumbnail_);

  static const std::int32_t ID = -158305341;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultVenue final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<venue> venue_;
  object_ptr<photoSize> thumbnail_;

  inlineQueryResultVenue();

  inlineQueryResultVenue(std::string const &id_, object_ptr<venue> &&venue_, object_ptr<photoSize> &&thumbnail_);

  static const std::int32_t ID = -1592932211;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultGame final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<game> game_;

  inlineQueryResultGame();

  inlineQueryResultGame(std::string const &id_, object_ptr<game> &&game_);

  static const std::int32_t ID = 1706916987;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultAnimation final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<animation> animation_;
  std::string title_;

  inlineQueryResultAnimation();

  inlineQueryResultAnimation(std::string const &id_, object_ptr<animation> &&animation_, std::string const &title_);

  static const std::int32_t ID = 2009984267;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultAudio final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<audio> audio_;

  inlineQueryResultAudio();

  inlineQueryResultAudio(std::string const &id_, object_ptr<audio> &&audio_);

  static const std::int32_t ID = 842650360;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultDocument final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<document> document_;
  std::string title_;
  std::string description_;

  inlineQueryResultDocument();

  inlineQueryResultDocument(std::string const &id_, object_ptr<document> &&document_, std::string const &title_, std::string const &description_);

  static const std::int32_t ID = -1491268539;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultPhoto final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<photo> photo_;
  std::string title_;
  std::string description_;

  inlineQueryResultPhoto();

  inlineQueryResultPhoto(std::string const &id_, object_ptr<photo> &&photo_, std::string const &title_, std::string const &description_);

  static const std::int32_t ID = 1848319440;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultSticker final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<sticker> sticker_;

  inlineQueryResultSticker();

  inlineQueryResultSticker(std::string const &id_, object_ptr<sticker> &&sticker_);

  static const std::int32_t ID = -1848224245;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultVideo final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<video> video_;
  std::string title_;
  std::string description_;

  inlineQueryResultVideo();

  inlineQueryResultVideo(std::string const &id_, object_ptr<video> &&video_, std::string const &title_, std::string const &description_);

  static const std::int32_t ID = -1373158683;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResultVoiceNote final : public InlineQueryResult {
 public:
  std::string id_;
  object_ptr<voiceNote> voice_note_;
  std::string title_;

  inlineQueryResultVoiceNote();

  inlineQueryResultVoiceNote(std::string const &id_, object_ptr<voiceNote> &&voice_note_, std::string const &title_);

  static const std::int32_t ID = -1897393105;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inlineQueryResults final : public Object {
 public:
  std::int64_t inline_query_id_;
  std::string next_offset_;
  std::vector<object_ptr<InlineQueryResult>> results_;
  std::string switch_pm_text_;
  std::string switch_pm_parameter_;

  inlineQueryResults();

  inlineQueryResults(std::int64_t inline_query_id_, std::string const &next_offset_, std::vector<object_ptr<InlineQueryResult>> &&results_, std::string const &switch_pm_text_, std::string const &switch_pm_parameter_);

  static const std::int32_t ID = 1000709656;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputBackground: public Object {
 public:
};

class inputBackgroundLocal final : public InputBackground {
 public:
  object_ptr<InputFile> background_;

  inputBackgroundLocal();

  explicit inputBackgroundLocal(object_ptr<InputFile> &&background_);

  static const std::int32_t ID = -1747094364;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputBackgroundRemote final : public InputBackground {
 public:
  std::int64_t background_id_;

  inputBackgroundRemote();

  explicit inputBackgroundRemote(std::int64_t background_id_);

  static const std::int32_t ID = -274976231;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputCredentials: public Object {
 public:
};

class inputCredentialsSaved final : public InputCredentials {
 public:
  std::string saved_credentials_id_;

  inputCredentialsSaved();

  explicit inputCredentialsSaved(std::string const &saved_credentials_id_);

  static const std::int32_t ID = -2034385364;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputCredentialsNew final : public InputCredentials {
 public:
  std::string data_;
  bool allow_save_;

  inputCredentialsNew();

  inputCredentialsNew(std::string const &data_, bool allow_save_);

  static const std::int32_t ID = -829689558;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputCredentialsAndroidPay final : public InputCredentials {
 public:
  std::string data_;

  inputCredentialsAndroidPay();

  explicit inputCredentialsAndroidPay(std::string const &data_);

  static const std::int32_t ID = 1979566832;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputCredentialsApplePay final : public InputCredentials {
 public:
  std::string data_;

  inputCredentialsApplePay();

  explicit inputCredentialsApplePay(std::string const &data_);

  static const std::int32_t ID = -1246570799;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputFile: public Object {
 public:
};

class inputFileId final : public InputFile {
 public:
  std::int32_t id_;

  inputFileId();

  explicit inputFileId(std::int32_t id_);

  static const std::int32_t ID = 1788906253;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputFileRemote final : public InputFile {
 public:
  std::string id_;

  inputFileRemote();

  explicit inputFileRemote(std::string const &id_);

  static const std::int32_t ID = -107574466;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputFileLocal final : public InputFile {
 public:
  std::string path_;

  inputFileLocal();

  explicit inputFileLocal(std::string const &path_);

  static const std::int32_t ID = 2056030919;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputFileGenerated final : public InputFile {
 public:
  std::string original_path_;
  std::string conversion_;
  std::int32_t expected_size_;

  inputFileGenerated();

  inputFileGenerated(std::string const &original_path_, std::string const &conversion_, std::int32_t expected_size_);

  static const std::int32_t ID = -1781351885;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputIdentityDocument final : public Object {
 public:
  std::string number_;
  object_ptr<date> expiry_date_;
  object_ptr<InputFile> front_side_;
  object_ptr<InputFile> reverse_side_;
  object_ptr<InputFile> selfie_;
  std::vector<object_ptr<InputFile>> translation_;

  inputIdentityDocument();

  inputIdentityDocument(std::string const &number_, object_ptr<date> &&expiry_date_, object_ptr<InputFile> &&front_side_, object_ptr<InputFile> &&reverse_side_, object_ptr<InputFile> &&selfie_, std::vector<object_ptr<InputFile>> &&translation_);

  static const std::int32_t ID = -381776063;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputInlineQueryResult: public Object {
 public:
};

class inputInlineQueryResultAnimatedGif final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string thumbnail_url_;
  std::string gif_url_;
  std::int32_t gif_duration_;
  std::int32_t gif_width_;
  std::int32_t gif_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultAnimatedGif();

  inputInlineQueryResultAnimatedGif(std::string const &id_, std::string const &title_, std::string const &thumbnail_url_, std::string const &gif_url_, std::int32_t gif_duration_, std::int32_t gif_width_, std::int32_t gif_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -891474894;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultAnimatedMpeg4 final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string thumbnail_url_;
  std::string mpeg4_url_;
  std::int32_t mpeg4_duration_;
  std::int32_t mpeg4_width_;
  std::int32_t mpeg4_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultAnimatedMpeg4();

  inputInlineQueryResultAnimatedMpeg4(std::string const &id_, std::string const &title_, std::string const &thumbnail_url_, std::string const &mpeg4_url_, std::int32_t mpeg4_duration_, std::int32_t mpeg4_width_, std::int32_t mpeg4_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1629529888;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultArticle final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string url_;
  bool hide_url_;
  std::string title_;
  std::string description_;
  std::string thumbnail_url_;
  std::int32_t thumbnail_width_;
  std::int32_t thumbnail_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultArticle();

  inputInlineQueryResultArticle(std::string const &id_, std::string const &url_, bool hide_url_, std::string const &title_, std::string const &description_, std::string const &thumbnail_url_, std::int32_t thumbnail_width_, std::int32_t thumbnail_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 1973670156;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultAudio final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string performer_;
  std::string audio_url_;
  std::int32_t audio_duration_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultAudio();

  inputInlineQueryResultAudio(std::string const &id_, std::string const &title_, std::string const &performer_, std::string const &audio_url_, std::int32_t audio_duration_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 1260139988;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultContact final : public InputInlineQueryResult {
 public:
  std::string id_;
  object_ptr<contact> contact_;
  std::string thumbnail_url_;
  std::int32_t thumbnail_width_;
  std::int32_t thumbnail_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultContact();

  inputInlineQueryResultContact(std::string const &id_, object_ptr<contact> &&contact_, std::string const &thumbnail_url_, std::int32_t thumbnail_width_, std::int32_t thumbnail_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 1846064594;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultDocument final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string description_;
  std::string document_url_;
  std::string mime_type_;
  std::string thumbnail_url_;
  std::int32_t thumbnail_width_;
  std::int32_t thumbnail_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultDocument();

  inputInlineQueryResultDocument(std::string const &id_, std::string const &title_, std::string const &description_, std::string const &document_url_, std::string const &mime_type_, std::string const &thumbnail_url_, std::int32_t thumbnail_width_, std::int32_t thumbnail_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 578801869;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultGame final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string game_short_name_;
  object_ptr<ReplyMarkup> reply_markup_;

  inputInlineQueryResultGame();

  inputInlineQueryResultGame(std::string const &id_, std::string const &game_short_name_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = 966074327;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultLocation final : public InputInlineQueryResult {
 public:
  std::string id_;
  object_ptr<location> location_;
  std::int32_t live_period_;
  std::string title_;
  std::string thumbnail_url_;
  std::int32_t thumbnail_width_;
  std::int32_t thumbnail_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultLocation();

  inputInlineQueryResultLocation(std::string const &id_, object_ptr<location> &&location_, std::int32_t live_period_, std::string const &title_, std::string const &thumbnail_url_, std::int32_t thumbnail_width_, std::int32_t thumbnail_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1887650218;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultPhoto final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string description_;
  std::string thumbnail_url_;
  std::string photo_url_;
  std::int32_t photo_width_;
  std::int32_t photo_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultPhoto();

  inputInlineQueryResultPhoto(std::string const &id_, std::string const &title_, std::string const &description_, std::string const &thumbnail_url_, std::string const &photo_url_, std::int32_t photo_width_, std::int32_t photo_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1123338721;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultSticker final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string thumbnail_url_;
  std::string sticker_url_;
  std::int32_t sticker_width_;
  std::int32_t sticker_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultSticker();

  inputInlineQueryResultSticker(std::string const &id_, std::string const &thumbnail_url_, std::string const &sticker_url_, std::int32_t sticker_width_, std::int32_t sticker_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 274007129;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultVenue final : public InputInlineQueryResult {
 public:
  std::string id_;
  object_ptr<venue> venue_;
  std::string thumbnail_url_;
  std::int32_t thumbnail_width_;
  std::int32_t thumbnail_height_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultVenue();

  inputInlineQueryResultVenue(std::string const &id_, object_ptr<venue> &&venue_, std::string const &thumbnail_url_, std::int32_t thumbnail_width_, std::int32_t thumbnail_height_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 541704509;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultVideo final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string description_;
  std::string thumbnail_url_;
  std::string video_url_;
  std::string mime_type_;
  std::int32_t video_width_;
  std::int32_t video_height_;
  std::int32_t video_duration_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultVideo();

  inputInlineQueryResultVideo(std::string const &id_, std::string const &title_, std::string const &description_, std::string const &thumbnail_url_, std::string const &video_url_, std::string const &mime_type_, std::int32_t video_width_, std::int32_t video_height_, std::int32_t video_duration_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 1724073191;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputInlineQueryResultVoiceNote final : public InputInlineQueryResult {
 public:
  std::string id_;
  std::string title_;
  std::string voice_note_url_;
  std::int32_t voice_note_duration_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  inputInlineQueryResultVoiceNote();

  inputInlineQueryResultVoiceNote(std::string const &id_, std::string const &title_, std::string const &voice_note_url_, std::int32_t voice_note_duration_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1790072503;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputMessageContent: public Object {
 public:
};

class inputMessageText final : public InputMessageContent {
 public:
  object_ptr<formattedText> text_;
  bool disable_web_page_preview_;
  bool clear_draft_;

  inputMessageText();

  inputMessageText(object_ptr<formattedText> &&text_, bool disable_web_page_preview_, bool clear_draft_);

  static const std::int32_t ID = 247050392;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageAnimation final : public InputMessageContent {
 public:
  object_ptr<InputFile> animation_;
  object_ptr<inputThumbnail> thumbnail_;
  std::int32_t duration_;
  std::int32_t width_;
  std::int32_t height_;
  object_ptr<formattedText> caption_;

  inputMessageAnimation();

  inputMessageAnimation(object_ptr<InputFile> &&animation_, object_ptr<inputThumbnail> &&thumbnail_, std::int32_t duration_, std::int32_t width_, std::int32_t height_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 926542724;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageAudio final : public InputMessageContent {
 public:
  object_ptr<InputFile> audio_;
  object_ptr<inputThumbnail> album_cover_thumbnail_;
  std::int32_t duration_;
  std::string title_;
  std::string performer_;
  object_ptr<formattedText> caption_;

  inputMessageAudio();

  inputMessageAudio(object_ptr<InputFile> &&audio_, object_ptr<inputThumbnail> &&album_cover_thumbnail_, std::int32_t duration_, std::string const &title_, std::string const &performer_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = -626786126;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageDocument final : public InputMessageContent {
 public:
  object_ptr<InputFile> document_;
  object_ptr<inputThumbnail> thumbnail_;
  object_ptr<formattedText> caption_;

  inputMessageDocument();

  inputMessageDocument(object_ptr<InputFile> &&document_, object_ptr<inputThumbnail> &&thumbnail_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 937970604;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessagePhoto final : public InputMessageContent {
 public:
  object_ptr<InputFile> photo_;
  object_ptr<inputThumbnail> thumbnail_;
  std::vector<std::int32_t> added_sticker_file_ids_;
  std::int32_t width_;
  std::int32_t height_;
  object_ptr<formattedText> caption_;
  std::int32_t ttl_;

  inputMessagePhoto();

  inputMessagePhoto(object_ptr<InputFile> &&photo_, object_ptr<inputThumbnail> &&thumbnail_, std::vector<std::int32_t> &&added_sticker_file_ids_, std::int32_t width_, std::int32_t height_, object_ptr<formattedText> &&caption_, std::int32_t ttl_);

  static const std::int32_t ID = 1648801584;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageSticker final : public InputMessageContent {
 public:
  object_ptr<InputFile> sticker_;
  object_ptr<inputThumbnail> thumbnail_;
  std::int32_t width_;
  std::int32_t height_;

  inputMessageSticker();

  inputMessageSticker(object_ptr<InputFile> &&sticker_, object_ptr<inputThumbnail> &&thumbnail_, std::int32_t width_, std::int32_t height_);

  static const std::int32_t ID = 740776325;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageVideo final : public InputMessageContent {
 public:
  object_ptr<InputFile> video_;
  object_ptr<inputThumbnail> thumbnail_;
  std::vector<std::int32_t> added_sticker_file_ids_;
  std::int32_t duration_;
  std::int32_t width_;
  std::int32_t height_;
  bool supports_streaming_;
  object_ptr<formattedText> caption_;
  std::int32_t ttl_;

  inputMessageVideo();

  inputMessageVideo(object_ptr<InputFile> &&video_, object_ptr<inputThumbnail> &&thumbnail_, std::vector<std::int32_t> &&added_sticker_file_ids_, std::int32_t duration_, std::int32_t width_, std::int32_t height_, bool supports_streaming_, object_ptr<formattedText> &&caption_, std::int32_t ttl_);

  static const std::int32_t ID = -2108486755;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageVideoNote final : public InputMessageContent {
 public:
  object_ptr<InputFile> video_note_;
  object_ptr<inputThumbnail> thumbnail_;
  std::int32_t duration_;
  std::int32_t length_;

  inputMessageVideoNote();

  inputMessageVideoNote(object_ptr<InputFile> &&video_note_, object_ptr<inputThumbnail> &&thumbnail_, std::int32_t duration_, std::int32_t length_);

  static const std::int32_t ID = 279108859;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageVoiceNote final : public InputMessageContent {
 public:
  object_ptr<InputFile> voice_note_;
  std::int32_t duration_;
  std::string waveform_;
  object_ptr<formattedText> caption_;

  inputMessageVoiceNote();

  inputMessageVoiceNote(object_ptr<InputFile> &&voice_note_, std::int32_t duration_, std::string const &waveform_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 2136519657;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageLocation final : public InputMessageContent {
 public:
  object_ptr<location> location_;
  std::int32_t live_period_;

  inputMessageLocation();

  inputMessageLocation(object_ptr<location> &&location_, std::int32_t live_period_);

  static const std::int32_t ID = -1624179655;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageVenue final : public InputMessageContent {
 public:
  object_ptr<venue> venue_;

  inputMessageVenue();

  explicit inputMessageVenue(object_ptr<venue> &&venue_);

  static const std::int32_t ID = 1447926269;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageContact final : public InputMessageContent {
 public:
  object_ptr<contact> contact_;

  inputMessageContact();

  explicit inputMessageContact(object_ptr<contact> &&contact_);

  static const std::int32_t ID = -982446849;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageGame final : public InputMessageContent {
 public:
  std::int32_t bot_user_id_;
  std::string game_short_name_;

  inputMessageGame();

  inputMessageGame(std::int32_t bot_user_id_, std::string const &game_short_name_);

  static const std::int32_t ID = -1728000914;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageInvoice final : public InputMessageContent {
 public:
  object_ptr<invoice> invoice_;
  std::string title_;
  std::string description_;
  std::string photo_url_;
  std::int32_t photo_size_;
  std::int32_t photo_width_;
  std::int32_t photo_height_;
  std::string payload_;
  std::string provider_token_;
  std::string provider_data_;
  std::string start_parameter_;

  inputMessageInvoice();

  inputMessageInvoice(object_ptr<invoice> &&invoice_, std::string const &title_, std::string const &description_, std::string const &photo_url_, std::int32_t photo_size_, std::int32_t photo_width_, std::int32_t photo_height_, std::string const &payload_, std::string const &provider_token_, std::string const &provider_data_, std::string const &start_parameter_);

  static const std::int32_t ID = 1038812175;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessagePoll final : public InputMessageContent {
 public:
  std::string question_;
  std::vector<std::string> options_;
  bool is_anonymous_;
  object_ptr<PollType> type_;
  bool is_closed_;

  inputMessagePoll();

  inputMessagePoll(std::string const &question_, std::vector<std::string> &&options_, bool is_anonymous_, object_ptr<PollType> &&type_, bool is_closed_);

  static const std::int32_t ID = 743307780;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputMessageForwarded final : public InputMessageContent {
 public:
  std::int64_t from_chat_id_;
  std::int64_t message_id_;
  bool in_game_share_;
  bool send_copy_;
  bool remove_caption_;

  inputMessageForwarded();

  inputMessageForwarded(std::int64_t from_chat_id_, std::int64_t message_id_, bool in_game_share_, bool send_copy_, bool remove_caption_);

  static const std::int32_t ID = 1503132333;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputPassportElement: public Object {
 public:
};

class inputPassportElementPersonalDetails final : public InputPassportElement {
 public:
  object_ptr<personalDetails> personal_details_;

  inputPassportElementPersonalDetails();

  explicit inputPassportElementPersonalDetails(object_ptr<personalDetails> &&personal_details_);

  static const std::int32_t ID = 164791359;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementPassport final : public InputPassportElement {
 public:
  object_ptr<inputIdentityDocument> passport_;

  inputPassportElementPassport();

  explicit inputPassportElementPassport(object_ptr<inputIdentityDocument> &&passport_);

  static const std::int32_t ID = -497011356;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementDriverLicense final : public InputPassportElement {
 public:
  object_ptr<inputIdentityDocument> driver_license_;

  inputPassportElementDriverLicense();

  explicit inputPassportElementDriverLicense(object_ptr<inputIdentityDocument> &&driver_license_);

  static const std::int32_t ID = 304813264;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementIdentityCard final : public InputPassportElement {
 public:
  object_ptr<inputIdentityDocument> identity_card_;

  inputPassportElementIdentityCard();

  explicit inputPassportElementIdentityCard(object_ptr<inputIdentityDocument> &&identity_card_);

  static const std::int32_t ID = -9963390;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementInternalPassport final : public InputPassportElement {
 public:
  object_ptr<inputIdentityDocument> internal_passport_;

  inputPassportElementInternalPassport();

  explicit inputPassportElementInternalPassport(object_ptr<inputIdentityDocument> &&internal_passport_);

  static const std::int32_t ID = 715360043;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementAddress final : public InputPassportElement {
 public:
  object_ptr<address> address_;

  inputPassportElementAddress();

  explicit inputPassportElementAddress(object_ptr<address> &&address_);

  static const std::int32_t ID = 461630480;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementUtilityBill final : public InputPassportElement {
 public:
  object_ptr<inputPersonalDocument> utility_bill_;

  inputPassportElementUtilityBill();

  explicit inputPassportElementUtilityBill(object_ptr<inputPersonalDocument> &&utility_bill_);

  static const std::int32_t ID = 1389203841;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementBankStatement final : public InputPassportElement {
 public:
  object_ptr<inputPersonalDocument> bank_statement_;

  inputPassportElementBankStatement();

  explicit inputPassportElementBankStatement(object_ptr<inputPersonalDocument> &&bank_statement_);

  static const std::int32_t ID = -26585208;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementRentalAgreement final : public InputPassportElement {
 public:
  object_ptr<inputPersonalDocument> rental_agreement_;

  inputPassportElementRentalAgreement();

  explicit inputPassportElementRentalAgreement(object_ptr<inputPersonalDocument> &&rental_agreement_);

  static const std::int32_t ID = 1736154155;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementPassportRegistration final : public InputPassportElement {
 public:
  object_ptr<inputPersonalDocument> passport_registration_;

  inputPassportElementPassportRegistration();

  explicit inputPassportElementPassportRegistration(object_ptr<inputPersonalDocument> &&passport_registration_);

  static const std::int32_t ID = 1314562128;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementTemporaryRegistration final : public InputPassportElement {
 public:
  object_ptr<inputPersonalDocument> temporary_registration_;

  inputPassportElementTemporaryRegistration();

  explicit inputPassportElementTemporaryRegistration(object_ptr<inputPersonalDocument> &&temporary_registration_);

  static const std::int32_t ID = -1913238047;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementPhoneNumber final : public InputPassportElement {
 public:
  std::string phone_number_;

  inputPassportElementPhoneNumber();

  explicit inputPassportElementPhoneNumber(std::string const &phone_number_);

  static const std::int32_t ID = 1319357497;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementEmailAddress final : public InputPassportElement {
 public:
  std::string email_address_;

  inputPassportElementEmailAddress();

  explicit inputPassportElementEmailAddress(std::string const &email_address_);

  static const std::int32_t ID = -248605659;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementError final : public Object {
 public:
  object_ptr<PassportElementType> type_;
  std::string message_;
  object_ptr<InputPassportElementErrorSource> source_;

  inputPassportElementError();

  inputPassportElementError(object_ptr<PassportElementType> &&type_, std::string const &message_, object_ptr<InputPassportElementErrorSource> &&source_);

  static const std::int32_t ID = 285756898;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class InputPassportElementErrorSource: public Object {
 public:
};

class inputPassportElementErrorSourceUnspecified final : public InputPassportElementErrorSource {
 public:
  std::string element_hash_;

  inputPassportElementErrorSourceUnspecified();

  explicit inputPassportElementErrorSourceUnspecified(std::string const &element_hash_);

  static const std::int32_t ID = 267230319;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceDataField final : public InputPassportElementErrorSource {
 public:
  std::string field_name_;
  std::string data_hash_;

  inputPassportElementErrorSourceDataField();

  inputPassportElementErrorSourceDataField(std::string const &field_name_, std::string const &data_hash_);

  static const std::int32_t ID = -426795002;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceFrontSide final : public InputPassportElementErrorSource {
 public:
  std::string file_hash_;

  inputPassportElementErrorSourceFrontSide();

  explicit inputPassportElementErrorSourceFrontSide(std::string const &file_hash_);

  static const std::int32_t ID = 588023741;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceReverseSide final : public InputPassportElementErrorSource {
 public:
  std::string file_hash_;

  inputPassportElementErrorSourceReverseSide();

  explicit inputPassportElementErrorSourceReverseSide(std::string const &file_hash_);

  static const std::int32_t ID = 413072891;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceSelfie final : public InputPassportElementErrorSource {
 public:
  std::string file_hash_;

  inputPassportElementErrorSourceSelfie();

  explicit inputPassportElementErrorSourceSelfie(std::string const &file_hash_);

  static const std::int32_t ID = -773575528;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceTranslationFile final : public InputPassportElementErrorSource {
 public:
  std::string file_hash_;

  inputPassportElementErrorSourceTranslationFile();

  explicit inputPassportElementErrorSourceTranslationFile(std::string const &file_hash_);

  static const std::int32_t ID = 505842299;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceTranslationFiles final : public InputPassportElementErrorSource {
 public:
  std::vector<std::string> file_hashes_;

  inputPassportElementErrorSourceTranslationFiles();

  explicit inputPassportElementErrorSourceTranslationFiles(std::vector<std::string> &&file_hashes_);

  static const std::int32_t ID = -527254048;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceFile final : public InputPassportElementErrorSource {
 public:
  std::string file_hash_;

  inputPassportElementErrorSourceFile();

  explicit inputPassportElementErrorSourceFile(std::string const &file_hash_);

  static const std::int32_t ID = -298492469;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPassportElementErrorSourceFiles final : public InputPassportElementErrorSource {
 public:
  std::vector<std::string> file_hashes_;

  inputPassportElementErrorSourceFiles();

  explicit inputPassportElementErrorSourceFiles(std::vector<std::string> &&file_hashes_);

  static const std::int32_t ID = -2008541640;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputPersonalDocument final : public Object {
 public:
  std::vector<object_ptr<InputFile>> files_;
  std::vector<object_ptr<InputFile>> translation_;

  inputPersonalDocument();

  inputPersonalDocument(std::vector<object_ptr<InputFile>> &&files_, std::vector<object_ptr<InputFile>> &&translation_);

  static const std::int32_t ID = 1676966826;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputSticker final : public Object {
 public:
  object_ptr<InputFile> png_sticker_;
  std::string emojis_;
  object_ptr<maskPosition> mask_position_;

  inputSticker();

  inputSticker(object_ptr<InputFile> &&png_sticker_, std::string const &emojis_, object_ptr<maskPosition> &&mask_position_);

  static const std::int32_t ID = -1998602205;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class inputThumbnail final : public Object {
 public:
  object_ptr<InputFile> thumbnail_;
  std::int32_t width_;
  std::int32_t height_;

  inputThumbnail();

  inputThumbnail(object_ptr<InputFile> &&thumbnail_, std::int32_t width_, std::int32_t height_);

  static const std::int32_t ID = 1582387236;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class invoice final : public Object {
 public:
  std::string currency_;
  std::vector<object_ptr<labeledPricePart>> price_parts_;
  bool is_test_;
  bool need_name_;
  bool need_phone_number_;
  bool need_email_address_;
  bool need_shipping_address_;
  bool send_phone_number_to_provider_;
  bool send_email_address_to_provider_;
  bool is_flexible_;

  invoice();

  invoice(std::string const &currency_, std::vector<object_ptr<labeledPricePart>> &&price_parts_, bool is_test_, bool need_name_, bool need_phone_number_, bool need_email_address_, bool need_shipping_address_, bool send_phone_number_to_provider_, bool send_email_address_to_provider_, bool is_flexible_);

  static const std::int32_t ID = -368451690;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonObjectMember final : public Object {
 public:
  std::string key_;
  object_ptr<JsonValue> value_;

  jsonObjectMember();

  jsonObjectMember(std::string const &key_, object_ptr<JsonValue> &&value_);

  static const std::int32_t ID = -1803309418;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class JsonValue: public Object {
 public:
};

class jsonValueNull final : public JsonValue {
 public:

  jsonValueNull();

  static const std::int32_t ID = -92872499;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonValueBoolean final : public JsonValue {
 public:
  bool value_;

  jsonValueBoolean();

  explicit jsonValueBoolean(bool value_);

  static const std::int32_t ID = -2142186576;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonValueNumber final : public JsonValue {
 public:
  double value_;

  jsonValueNumber();

  explicit jsonValueNumber(double value_);

  static const std::int32_t ID = -1010822033;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonValueString final : public JsonValue {
 public:
  std::string value_;

  jsonValueString();

  explicit jsonValueString(std::string const &value_);

  static const std::int32_t ID = 1597947313;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonValueArray final : public JsonValue {
 public:
  std::vector<object_ptr<JsonValue>> values_;

  jsonValueArray();

  explicit jsonValueArray(std::vector<object_ptr<JsonValue>> &&values_);

  static const std::int32_t ID = -183913546;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class jsonValueObject final : public JsonValue {
 public:
  std::vector<object_ptr<jsonObjectMember>> members_;

  jsonValueObject();

  explicit jsonValueObject(std::vector<object_ptr<jsonObjectMember>> &&members_);

  static const std::int32_t ID = 520252026;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class keyboardButton final : public Object {
 public:
  std::string text_;
  object_ptr<KeyboardButtonType> type_;

  keyboardButton();

  keyboardButton(std::string const &text_, object_ptr<KeyboardButtonType> &&type_);

  static const std::int32_t ID = -2069836172;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class KeyboardButtonType: public Object {
 public:
};

class keyboardButtonTypeText final : public KeyboardButtonType {
 public:

  keyboardButtonTypeText();

  static const std::int32_t ID = -1773037256;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class keyboardButtonTypeRequestPhoneNumber final : public KeyboardButtonType {
 public:

  keyboardButtonTypeRequestPhoneNumber();

  static const std::int32_t ID = -1529235527;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class keyboardButtonTypeRequestLocation final : public KeyboardButtonType {
 public:

  keyboardButtonTypeRequestLocation();

  static const std::int32_t ID = -125661955;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class keyboardButtonTypeRequestPoll final : public KeyboardButtonType {
 public:
  bool force_regular_;
  bool force_quiz_;

  keyboardButtonTypeRequestPoll();

  keyboardButtonTypeRequestPoll(bool force_regular_, bool force_quiz_);

  static const std::int32_t ID = 1902435512;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class labeledPricePart final : public Object {
 public:
  std::string label_;
  std::int64_t amount_;

  labeledPricePart();

  labeledPricePart(std::string const &label_, std::int64_t amount_);

  static const std::int32_t ID = 552789798;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class languagePackInfo final : public Object {
 public:
  std::string id_;
  std::string base_language_pack_id_;
  std::string name_;
  std::string native_name_;
  std::string plural_code_;
  bool is_official_;
  bool is_rtl_;
  bool is_beta_;
  bool is_installed_;
  std::int32_t total_string_count_;
  std::int32_t translated_string_count_;
  std::int32_t local_string_count_;
  std::string translation_url_;

  languagePackInfo();

  languagePackInfo(std::string const &id_, std::string const &base_language_pack_id_, std::string const &name_, std::string const &native_name_, std::string const &plural_code_, bool is_official_, bool is_rtl_, bool is_beta_, bool is_installed_, std::int32_t total_string_count_, std::int32_t translated_string_count_, std::int32_t local_string_count_, std::string const &translation_url_);

  static const std::int32_t ID = 542199642;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class languagePackString final : public Object {
 public:
  std::string key_;
  object_ptr<LanguagePackStringValue> value_;

  languagePackString();

  languagePackString(std::string const &key_, object_ptr<LanguagePackStringValue> &&value_);

  static const std::int32_t ID = 1307632736;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class LanguagePackStringValue: public Object {
 public:
};

class languagePackStringValueOrdinary final : public LanguagePackStringValue {
 public:
  std::string value_;

  languagePackStringValueOrdinary();

  explicit languagePackStringValueOrdinary(std::string const &value_);

  static const std::int32_t ID = -249256352;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class languagePackStringValuePluralized final : public LanguagePackStringValue {
 public:
  std::string zero_value_;
  std::string one_value_;
  std::string two_value_;
  std::string few_value_;
  std::string many_value_;
  std::string other_value_;

  languagePackStringValuePluralized();

  languagePackStringValuePluralized(std::string const &zero_value_, std::string const &one_value_, std::string const &two_value_, std::string const &few_value_, std::string const &many_value_, std::string const &other_value_);

  static const std::int32_t ID = 1906840261;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class languagePackStringValueDeleted final : public LanguagePackStringValue {
 public:

  languagePackStringValueDeleted();

  static const std::int32_t ID = 1834792698;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class languagePackStrings final : public Object {
 public:
  std::vector<object_ptr<languagePackString>> strings_;

  languagePackStrings();

  explicit languagePackStrings(std::vector<object_ptr<languagePackString>> &&strings_);

  static const std::int32_t ID = 1172082922;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class localFile final : public Object {
 public:
  std::string path_;
  bool can_be_downloaded_;
  bool can_be_deleted_;
  bool is_downloading_active_;
  bool is_downloading_completed_;
  std::int32_t download_offset_;
  std::int32_t downloaded_prefix_size_;
  std::int32_t downloaded_size_;

  localFile();

  localFile(std::string const &path_, bool can_be_downloaded_, bool can_be_deleted_, bool is_downloading_active_, bool is_downloading_completed_, std::int32_t download_offset_, std::int32_t downloaded_prefix_size_, std::int32_t downloaded_size_);

  static const std::int32_t ID = -1166400317;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class localizationTargetInfo final : public Object {
 public:
  std::vector<object_ptr<languagePackInfo>> language_packs_;

  localizationTargetInfo();

  explicit localizationTargetInfo(std::vector<object_ptr<languagePackInfo>> &&language_packs_);

  static const std::int32_t ID = -2048670809;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class location final : public Object {
 public:
  double latitude_;
  double longitude_;

  location();

  location(double latitude_, double longitude_);

  static const std::int32_t ID = 749028016;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class LogStream: public Object {
 public:
};

class logStreamDefault final : public LogStream {
 public:

  logStreamDefault();

  static const std::int32_t ID = 1390581436;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class logStreamFile final : public LogStream {
 public:
  std::string path_;
  std::int64_t max_file_size_;

  logStreamFile();

  logStreamFile(std::string const &path_, std::int64_t max_file_size_);

  static const std::int32_t ID = -1880085930;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class logStreamEmpty final : public LogStream {
 public:

  logStreamEmpty();

  static const std::int32_t ID = -499912244;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class logTags final : public Object {
 public:
  std::vector<std::string> tags_;

  logTags();

  explicit logTags(std::vector<std::string> &&tags_);

  static const std::int32_t ID = -1604930601;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class logVerbosityLevel final : public Object {
 public:
  std::int32_t verbosity_level_;

  logVerbosityLevel();

  explicit logVerbosityLevel(std::int32_t verbosity_level_);

  static const std::int32_t ID = 1734624234;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class LoginUrlInfo: public Object {
 public:
};

class loginUrlInfoOpen final : public LoginUrlInfo {
 public:
  std::string url_;
  bool skip_confirm_;

  loginUrlInfoOpen();

  loginUrlInfoOpen(std::string const &url_, bool skip_confirm_);

  static const std::int32_t ID = -1079045420;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class loginUrlInfoRequestConfirmation final : public LoginUrlInfo {
 public:
  std::string url_;
  std::string domain_;
  std::int32_t bot_user_id_;
  bool request_write_access_;

  loginUrlInfoRequestConfirmation();

  loginUrlInfoRequestConfirmation(std::string const &url_, std::string const &domain_, std::int32_t bot_user_id_, bool request_write_access_);

  static const std::int32_t ID = -1761898342;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class MaskPoint: public Object {
 public:
};

class maskPointForehead final : public MaskPoint {
 public:

  maskPointForehead();

  static const std::int32_t ID = 1027512005;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class maskPointEyes final : public MaskPoint {
 public:

  maskPointEyes();

  static const std::int32_t ID = 1748310861;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class maskPointMouth final : public MaskPoint {
 public:

  maskPointMouth();

  static const std::int32_t ID = 411773406;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class maskPointChin final : public MaskPoint {
 public:

  maskPointChin();

  static const std::int32_t ID = 534995335;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class maskPosition final : public Object {
 public:
  object_ptr<MaskPoint> point_;
  double x_shift_;
  double y_shift_;
  double scale_;

  maskPosition();

  maskPosition(object_ptr<MaskPoint> &&point_, double x_shift_, double y_shift_, double scale_);

  static const std::int32_t ID = -2097433026;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class message final : public Object {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  std::int64_t chat_id_;
  object_ptr<MessageSendingState> sending_state_;
  object_ptr<MessageSchedulingState> scheduling_state_;
  bool is_outgoing_;
  bool can_be_edited_;
  bool can_be_forwarded_;
  bool can_be_deleted_only_for_self_;
  bool can_be_deleted_for_all_users_;
  bool is_channel_post_;
  bool contains_unread_mention_;
  std::int32_t date_;
  std::int32_t edit_date_;
  object_ptr<messageForwardInfo> forward_info_;
  std::int64_t reply_to_message_id_;
  std::int32_t ttl_;
  double ttl_expires_in_;
  std::int32_t via_bot_user_id_;
  std::string author_signature_;
  std::int32_t views_;
  std::int64_t media_album_id_;
  std::string restriction_reason_;
  object_ptr<MessageContent> content_;
  object_ptr<ReplyMarkup> reply_markup_;

  message();

  message(std::int64_t id_, std::int32_t sender_user_id_, std::int64_t chat_id_, object_ptr<MessageSendingState> &&sending_state_, object_ptr<MessageSchedulingState> &&scheduling_state_, bool is_outgoing_, bool can_be_edited_, bool can_be_forwarded_, bool can_be_deleted_only_for_self_, bool can_be_deleted_for_all_users_, bool is_channel_post_, bool contains_unread_mention_, std::int32_t date_, std::int32_t edit_date_, object_ptr<messageForwardInfo> &&forward_info_, std::int64_t reply_to_message_id_, std::int32_t ttl_, double ttl_expires_in_, std::int32_t via_bot_user_id_, std::string const &author_signature_, std::int32_t views_, std::int64_t media_album_id_, std::string const &restriction_reason_, object_ptr<MessageContent> &&content_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = 1169109781;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class MessageContent: public Object {
 public:
};

class messageText final : public MessageContent {
 public:
  object_ptr<formattedText> text_;
  object_ptr<webPage> web_page_;

  messageText();

  messageText(object_ptr<formattedText> &&text_, object_ptr<webPage> &&web_page_);

  static const std::int32_t ID = 1989037971;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageAnimation final : public MessageContent {
 public:
  object_ptr<animation> animation_;
  object_ptr<formattedText> caption_;
  bool is_secret_;

  messageAnimation();

  messageAnimation(object_ptr<animation> &&animation_, object_ptr<formattedText> &&caption_, bool is_secret_);

  static const std::int32_t ID = 1306939396;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageAudio final : public MessageContent {
 public:
  object_ptr<audio> audio_;
  object_ptr<formattedText> caption_;

  messageAudio();

  messageAudio(object_ptr<audio> &&audio_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 276722716;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageDocument final : public MessageContent {
 public:
  object_ptr<document> document_;
  object_ptr<formattedText> caption_;

  messageDocument();

  messageDocument(object_ptr<document> &&document_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 596945783;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePhoto final : public MessageContent {
 public:
  object_ptr<photo> photo_;
  object_ptr<formattedText> caption_;
  bool is_secret_;

  messagePhoto();

  messagePhoto(object_ptr<photo> &&photo_, object_ptr<formattedText> &&caption_, bool is_secret_);

  static const std::int32_t ID = -1851395174;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageExpiredPhoto final : public MessageContent {
 public:

  messageExpiredPhoto();

  static const std::int32_t ID = -1404641801;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageSticker final : public MessageContent {
 public:
  object_ptr<sticker> sticker_;

  messageSticker();

  explicit messageSticker(object_ptr<sticker> &&sticker_);

  static const std::int32_t ID = 1779022878;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageVideo final : public MessageContent {
 public:
  object_ptr<video> video_;
  object_ptr<formattedText> caption_;
  bool is_secret_;

  messageVideo();

  messageVideo(object_ptr<video> &&video_, object_ptr<formattedText> &&caption_, bool is_secret_);

  static const std::int32_t ID = 2021281344;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageExpiredVideo final : public MessageContent {
 public:

  messageExpiredVideo();

  static const std::int32_t ID = -1212209981;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageVideoNote final : public MessageContent {
 public:
  object_ptr<videoNote> video_note_;
  bool is_viewed_;
  bool is_secret_;

  messageVideoNote();

  messageVideoNote(object_ptr<videoNote> &&video_note_, bool is_viewed_, bool is_secret_);

  static const std::int32_t ID = 963323014;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageVoiceNote final : public MessageContent {
 public:
  object_ptr<voiceNote> voice_note_;
  object_ptr<formattedText> caption_;
  bool is_listened_;

  messageVoiceNote();

  messageVoiceNote(object_ptr<voiceNote> &&voice_note_, object_ptr<formattedText> &&caption_, bool is_listened_);

  static const std::int32_t ID = 527777781;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageLocation final : public MessageContent {
 public:
  object_ptr<location> location_;
  std::int32_t live_period_;
  std::int32_t expires_in_;

  messageLocation();

  messageLocation(object_ptr<location> &&location_, std::int32_t live_period_, std::int32_t expires_in_);

  static const std::int32_t ID = -1301887786;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageVenue final : public MessageContent {
 public:
  object_ptr<venue> venue_;

  messageVenue();

  explicit messageVenue(object_ptr<venue> &&venue_);

  static const std::int32_t ID = -2146492043;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageContact final : public MessageContent {
 public:
  object_ptr<contact> contact_;

  messageContact();

  explicit messageContact(object_ptr<contact> &&contact_);

  static const std::int32_t ID = -512684966;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageGame final : public MessageContent {
 public:
  object_ptr<game> game_;

  messageGame();

  explicit messageGame(object_ptr<game> &&game_);

  static const std::int32_t ID = -69441162;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePoll final : public MessageContent {
 public:
  object_ptr<poll> poll_;

  messagePoll();

  explicit messagePoll(object_ptr<poll> &&poll_);

  static const std::int32_t ID = -662130099;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageInvoice final : public MessageContent {
 public:
  std::string title_;
  std::string description_;
  object_ptr<photo> photo_;
  std::string currency_;
  std::int64_t total_amount_;
  std::string start_parameter_;
  bool is_test_;
  bool need_shipping_address_;
  std::int64_t receipt_message_id_;

  messageInvoice();

  messageInvoice(std::string const &title_, std::string const &description_, object_ptr<photo> &&photo_, std::string const &currency_, std::int64_t total_amount_, std::string const &start_parameter_, bool is_test_, bool need_shipping_address_, std::int64_t receipt_message_id_);

  static const std::int32_t ID = -1916671476;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageCall final : public MessageContent {
 public:
  object_ptr<CallDiscardReason> discard_reason_;
  std::int32_t duration_;

  messageCall();

  messageCall(object_ptr<CallDiscardReason> &&discard_reason_, std::int32_t duration_);

  static const std::int32_t ID = 366512596;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageBasicGroupChatCreate final : public MessageContent {
 public:
  std::string title_;
  std::vector<std::int32_t> member_user_ids_;

  messageBasicGroupChatCreate();

  messageBasicGroupChatCreate(std::string const &title_, std::vector<std::int32_t> &&member_user_ids_);

  static const std::int32_t ID = 1575377646;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageSupergroupChatCreate final : public MessageContent {
 public:
  std::string title_;

  messageSupergroupChatCreate();

  explicit messageSupergroupChatCreate(std::string const &title_);

  static const std::int32_t ID = -434325733;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatChangeTitle final : public MessageContent {
 public:
  std::string title_;

  messageChatChangeTitle();

  explicit messageChatChangeTitle(std::string const &title_);

  static const std::int32_t ID = 748272449;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatChangePhoto final : public MessageContent {
 public:
  object_ptr<photo> photo_;

  messageChatChangePhoto();

  explicit messageChatChangePhoto(object_ptr<photo> &&photo_);

  static const std::int32_t ID = 319630249;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatDeletePhoto final : public MessageContent {
 public:

  messageChatDeletePhoto();

  static const std::int32_t ID = -184374809;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatAddMembers final : public MessageContent {
 public:
  std::vector<std::int32_t> member_user_ids_;

  messageChatAddMembers();

  explicit messageChatAddMembers(std::vector<std::int32_t> &&member_user_ids_);

  static const std::int32_t ID = 401228326;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatJoinByLink final : public MessageContent {
 public:

  messageChatJoinByLink();

  static const std::int32_t ID = 1846493311;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatDeleteMember final : public MessageContent {
 public:
  std::int32_t user_id_;

  messageChatDeleteMember();

  explicit messageChatDeleteMember(std::int32_t user_id_);

  static const std::int32_t ID = 1164414043;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatUpgradeTo final : public MessageContent {
 public:
  std::int32_t supergroup_id_;

  messageChatUpgradeTo();

  explicit messageChatUpgradeTo(std::int32_t supergroup_id_);

  static const std::int32_t ID = 1957816681;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatUpgradeFrom final : public MessageContent {
 public:
  std::string title_;
  std::int32_t basic_group_id_;

  messageChatUpgradeFrom();

  messageChatUpgradeFrom(std::string const &title_, std::int32_t basic_group_id_);

  static const std::int32_t ID = 1642272558;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePinMessage final : public MessageContent {
 public:
  std::int64_t message_id_;

  messagePinMessage();

  explicit messagePinMessage(std::int64_t message_id_);

  static const std::int32_t ID = 953503801;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageScreenshotTaken final : public MessageContent {
 public:

  messageScreenshotTaken();

  static const std::int32_t ID = -1564971605;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageChatSetTtl final : public MessageContent {
 public:
  std::int32_t ttl_;

  messageChatSetTtl();

  explicit messageChatSetTtl(std::int32_t ttl_);

  static const std::int32_t ID = 1810060209;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageCustomServiceAction final : public MessageContent {
 public:
  std::string text_;

  messageCustomServiceAction();

  explicit messageCustomServiceAction(std::string const &text_);

  static const std::int32_t ID = 1435879282;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageGameScore final : public MessageContent {
 public:
  std::int64_t game_message_id_;
  std::int64_t game_id_;
  std::int32_t score_;

  messageGameScore();

  messageGameScore(std::int64_t game_message_id_, std::int64_t game_id_, std::int32_t score_);

  static const std::int32_t ID = 1344904575;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePaymentSuccessful final : public MessageContent {
 public:
  std::int64_t invoice_message_id_;
  std::string currency_;
  std::int64_t total_amount_;

  messagePaymentSuccessful();

  messagePaymentSuccessful(std::int64_t invoice_message_id_, std::string const &currency_, std::int64_t total_amount_);

  static const std::int32_t ID = -595962993;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePaymentSuccessfulBot final : public MessageContent {
 public:
  std::int64_t invoice_message_id_;
  std::string currency_;
  std::int64_t total_amount_;
  std::string invoice_payload_;
  std::string shipping_option_id_;
  object_ptr<orderInfo> order_info_;
  std::string telegram_payment_charge_id_;
  std::string provider_payment_charge_id_;

  messagePaymentSuccessfulBot();

  messagePaymentSuccessfulBot(std::int64_t invoice_message_id_, std::string const &currency_, std::int64_t total_amount_, std::string const &invoice_payload_, std::string const &shipping_option_id_, object_ptr<orderInfo> &&order_info_, std::string const &telegram_payment_charge_id_, std::string const &provider_payment_charge_id_);

  static const std::int32_t ID = -412310696;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageContactRegistered final : public MessageContent {
 public:

  messageContactRegistered();

  static const std::int32_t ID = -1502020353;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageWebsiteConnected final : public MessageContent {
 public:
  std::string domain_name_;

  messageWebsiteConnected();

  explicit messageWebsiteConnected(std::string const &domain_name_);

  static const std::int32_t ID = -1074551800;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePassportDataSent final : public MessageContent {
 public:
  std::vector<object_ptr<PassportElementType>> types_;

  messagePassportDataSent();

  explicit messagePassportDataSent(std::vector<object_ptr<PassportElementType>> &&types_);

  static const std::int32_t ID = 1017405171;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messagePassportDataReceived final : public MessageContent {
 public:
  std::vector<object_ptr<encryptedPassportElement>> elements_;
  object_ptr<encryptedCredentials> credentials_;

  messagePassportDataReceived();

  messagePassportDataReceived(std::vector<object_ptr<encryptedPassportElement>> &&elements_, object_ptr<encryptedCredentials> &&credentials_);

  static const std::int32_t ID = -1367863624;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageUnsupported final : public MessageContent {
 public:

  messageUnsupported();

  static const std::int32_t ID = -1816726139;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageForwardInfo final : public Object {
 public:
  object_ptr<MessageForwardOrigin> origin_;
  std::int32_t date_;
  std::int64_t from_chat_id_;
  std::int64_t from_message_id_;

  messageForwardInfo();

  messageForwardInfo(object_ptr<MessageForwardOrigin> &&origin_, std::int32_t date_, std::int64_t from_chat_id_, std::int64_t from_message_id_);

  static const std::int32_t ID = -1622371186;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class MessageForwardOrigin: public Object {
 public:
};

class messageForwardOriginUser final : public MessageForwardOrigin {
 public:
  std::int32_t sender_user_id_;

  messageForwardOriginUser();

  explicit messageForwardOriginUser(std::int32_t sender_user_id_);

  static const std::int32_t ID = 2781520;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageForwardOriginHiddenUser final : public MessageForwardOrigin {
 public:
  std::string sender_name_;

  messageForwardOriginHiddenUser();

  explicit messageForwardOriginHiddenUser(std::string const &sender_name_);

  static const std::int32_t ID = -271257885;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageForwardOriginChannel final : public MessageForwardOrigin {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::string author_signature_;

  messageForwardOriginChannel();

  messageForwardOriginChannel(std::int64_t chat_id_, std::int64_t message_id_, std::string const &author_signature_);

  static const std::int32_t ID = 1490730723;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageLinkInfo final : public Object {
 public:
  bool is_public_;
  std::int64_t chat_id_;
  object_ptr<message> message_;
  bool for_album_;

  messageLinkInfo();

  messageLinkInfo(bool is_public_, std::int64_t chat_id_, object_ptr<message> &&message_, bool for_album_);

  static const std::int32_t ID = 657372995;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class MessageSchedulingState: public Object {
 public:
};

class messageSchedulingStateSendAtDate final : public MessageSchedulingState {
 public:
  std::int32_t send_date_;

  messageSchedulingStateSendAtDate();

  explicit messageSchedulingStateSendAtDate(std::int32_t send_date_);

  static const std::int32_t ID = -1485570073;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageSchedulingStateSendWhenOnline final : public MessageSchedulingState {
 public:

  messageSchedulingStateSendWhenOnline();

  static const std::int32_t ID = 2092947464;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class MessageSendingState: public Object {
 public:
};

class messageSendingStatePending final : public MessageSendingState {
 public:

  messageSendingStatePending();

  static const std::int32_t ID = -1381803582;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messageSendingStateFailed final : public MessageSendingState {
 public:
  std::int32_t error_code_;
  std::string error_message_;
  bool can_retry_;
  double retry_after_;

  messageSendingStateFailed();

  messageSendingStateFailed(std::int32_t error_code_, std::string const &error_message_, bool can_retry_, double retry_after_);

  static const std::int32_t ID = 2054476087;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class messages final : public Object {
 public:
  std::int32_t total_count_;
  std::vector<object_ptr<message>> messages_;

  messages();

  messages(std::int32_t total_count_, std::vector<object_ptr<message>> &&messages_);

  static const std::int32_t ID = -16498159;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class minithumbnail final : public Object {
 public:
  std::int32_t width_;
  std::int32_t height_;
  std::string data_;

  minithumbnail();

  minithumbnail(std::int32_t width_, std::int32_t height_, std::string const &data_);

  static const std::int32_t ID = -328540758;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkStatistics final : public Object {
 public:
  std::int32_t since_date_;
  std::vector<object_ptr<NetworkStatisticsEntry>> entries_;

  networkStatistics();

  networkStatistics(std::int32_t since_date_, std::vector<object_ptr<NetworkStatisticsEntry>> &&entries_);

  static const std::int32_t ID = 1615554212;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class NetworkStatisticsEntry: public Object {
 public:
};

class networkStatisticsEntryFile final : public NetworkStatisticsEntry {
 public:
  object_ptr<FileType> file_type_;
  object_ptr<NetworkType> network_type_;
  std::int64_t sent_bytes_;
  std::int64_t received_bytes_;

  networkStatisticsEntryFile();

  networkStatisticsEntryFile(object_ptr<FileType> &&file_type_, object_ptr<NetworkType> &&network_type_, std::int64_t sent_bytes_, std::int64_t received_bytes_);

  static const std::int32_t ID = 188452706;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkStatisticsEntryCall final : public NetworkStatisticsEntry {
 public:
  object_ptr<NetworkType> network_type_;
  std::int64_t sent_bytes_;
  std::int64_t received_bytes_;
  double duration_;

  networkStatisticsEntryCall();

  networkStatisticsEntryCall(object_ptr<NetworkType> &&network_type_, std::int64_t sent_bytes_, std::int64_t received_bytes_, double duration_);

  static const std::int32_t ID = 737000365;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class NetworkType: public Object {
 public:
};

class networkTypeNone final : public NetworkType {
 public:

  networkTypeNone();

  static const std::int32_t ID = -1971691759;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkTypeMobile final : public NetworkType {
 public:

  networkTypeMobile();

  static const std::int32_t ID = 819228239;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkTypeMobileRoaming final : public NetworkType {
 public:

  networkTypeMobileRoaming();

  static const std::int32_t ID = -1435199760;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkTypeWiFi final : public NetworkType {
 public:

  networkTypeWiFi();

  static const std::int32_t ID = -633872070;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class networkTypeOther final : public NetworkType {
 public:

  networkTypeOther();

  static const std::int32_t ID = 1942128539;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notification final : public Object {
 public:
  std::int32_t id_;
  std::int32_t date_;
  bool is_silent_;
  object_ptr<NotificationType> type_;

  notification();

  notification(std::int32_t id_, std::int32_t date_, bool is_silent_, object_ptr<NotificationType> &&type_);

  static const std::int32_t ID = 788743120;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationGroup final : public Object {
 public:
  std::int32_t id_;
  object_ptr<NotificationGroupType> type_;
  std::int64_t chat_id_;
  std::int32_t total_count_;
  std::vector<object_ptr<notification>> notifications_;

  notificationGroup();

  notificationGroup(std::int32_t id_, object_ptr<NotificationGroupType> &&type_, std::int64_t chat_id_, std::int32_t total_count_, std::vector<object_ptr<notification>> &&notifications_);

  static const std::int32_t ID = 780691541;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class NotificationGroupType: public Object {
 public:
};

class notificationGroupTypeMessages final : public NotificationGroupType {
 public:

  notificationGroupTypeMessages();

  static const std::int32_t ID = -1702481123;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationGroupTypeMentions final : public NotificationGroupType {
 public:

  notificationGroupTypeMentions();

  static const std::int32_t ID = -2050324051;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationGroupTypeSecretChat final : public NotificationGroupType {
 public:

  notificationGroupTypeSecretChat();

  static const std::int32_t ID = 1390759476;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationGroupTypeCalls final : public NotificationGroupType {
 public:

  notificationGroupTypeCalls();

  static const std::int32_t ID = 1379123538;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class NotificationSettingsScope: public Object {
 public:
};

class notificationSettingsScopePrivateChats final : public NotificationSettingsScope {
 public:

  notificationSettingsScopePrivateChats();

  static const std::int32_t ID = 937446759;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationSettingsScopeGroupChats final : public NotificationSettingsScope {
 public:

  notificationSettingsScopeGroupChats();

  static const std::int32_t ID = 1212142067;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationSettingsScopeChannelChats final : public NotificationSettingsScope {
 public:

  notificationSettingsScopeChannelChats();

  static const std::int32_t ID = 548013448;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class NotificationType: public Object {
 public:
};

class notificationTypeNewMessage final : public NotificationType {
 public:
  object_ptr<message> message_;

  notificationTypeNewMessage();

  explicit notificationTypeNewMessage(object_ptr<message> &&message_);

  static const std::int32_t ID = 1885935159;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationTypeNewSecretChat final : public NotificationType {
 public:

  notificationTypeNewSecretChat();

  static const std::int32_t ID = 1198638768;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationTypeNewCall final : public NotificationType {
 public:
  std::int32_t call_id_;

  notificationTypeNewCall();

  explicit notificationTypeNewCall(std::int32_t call_id_);

  static const std::int32_t ID = 1712734585;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class notificationTypeNewPushMessage final : public NotificationType {
 public:
  std::int64_t message_id_;
  std::int32_t sender_user_id_;
  object_ptr<PushMessageContent> content_;

  notificationTypeNewPushMessage();

  notificationTypeNewPushMessage(std::int64_t message_id_, std::int32_t sender_user_id_, object_ptr<PushMessageContent> &&content_);

  static const std::int32_t ID = 1167232404;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ok final : public Object {
 public:

  ok();

  static const std::int32_t ID = -722616727;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class OptionValue: public Object {
 public:
};

class optionValueBoolean final : public OptionValue {
 public:
  bool value_;

  optionValueBoolean();

  explicit optionValueBoolean(bool value_);

  static const std::int32_t ID = 63135518;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class optionValueEmpty final : public OptionValue {
 public:

  optionValueEmpty();

  static const std::int32_t ID = 918955155;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class optionValueInteger final : public OptionValue {
 public:
  std::int32_t value_;

  optionValueInteger();

  explicit optionValueInteger(std::int32_t value_);

  static const std::int32_t ID = -1400911104;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class optionValueString final : public OptionValue {
 public:
  std::string value_;

  optionValueString();

  explicit optionValueString(std::string const &value_);

  static const std::int32_t ID = 756248212;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class orderInfo final : public Object {
 public:
  std::string name_;
  std::string phone_number_;
  std::string email_address_;
  object_ptr<address> shipping_address_;

  orderInfo();

  orderInfo(std::string const &name_, std::string const &phone_number_, std::string const &email_address_, object_ptr<address> &&shipping_address_);

  static const std::int32_t ID = 783997294;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PageBlock: public Object {
 public:
};

class pageBlockTitle final : public PageBlock {
 public:
  object_ptr<RichText> title_;

  pageBlockTitle();

  explicit pageBlockTitle(object_ptr<RichText> &&title_);

  static const std::int32_t ID = 1629664784;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockSubtitle final : public PageBlock {
 public:
  object_ptr<RichText> subtitle_;

  pageBlockSubtitle();

  explicit pageBlockSubtitle(object_ptr<RichText> &&subtitle_);

  static const std::int32_t ID = 264524263;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockAuthorDate final : public PageBlock {
 public:
  object_ptr<RichText> author_;
  std::int32_t publish_date_;

  pageBlockAuthorDate();

  pageBlockAuthorDate(object_ptr<RichText> &&author_, std::int32_t publish_date_);

  static const std::int32_t ID = 1300231184;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockHeader final : public PageBlock {
 public:
  object_ptr<RichText> header_;

  pageBlockHeader();

  explicit pageBlockHeader(object_ptr<RichText> &&header_);

  static const std::int32_t ID = 1402854811;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockSubheader final : public PageBlock {
 public:
  object_ptr<RichText> subheader_;

  pageBlockSubheader();

  explicit pageBlockSubheader(object_ptr<RichText> &&subheader_);

  static const std::int32_t ID = 1263956774;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockKicker final : public PageBlock {
 public:
  object_ptr<RichText> kicker_;

  pageBlockKicker();

  explicit pageBlockKicker(object_ptr<RichText> &&kicker_);

  static const std::int32_t ID = 1361282635;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockParagraph final : public PageBlock {
 public:
  object_ptr<RichText> text_;

  pageBlockParagraph();

  explicit pageBlockParagraph(object_ptr<RichText> &&text_);

  static const std::int32_t ID = 1182402406;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockPreformatted final : public PageBlock {
 public:
  object_ptr<RichText> text_;
  std::string language_;

  pageBlockPreformatted();

  pageBlockPreformatted(object_ptr<RichText> &&text_, std::string const &language_);

  static const std::int32_t ID = -1066346178;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockFooter final : public PageBlock {
 public:
  object_ptr<RichText> footer_;

  pageBlockFooter();

  explicit pageBlockFooter(object_ptr<RichText> &&footer_);

  static const std::int32_t ID = 886429480;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockDivider final : public PageBlock {
 public:

  pageBlockDivider();

  static const std::int32_t ID = -618614392;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockAnchor final : public PageBlock {
 public:
  std::string name_;

  pageBlockAnchor();

  explicit pageBlockAnchor(std::string const &name_);

  static const std::int32_t ID = -837994576;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockList final : public PageBlock {
 public:
  std::vector<object_ptr<pageBlockListItem>> items_;

  pageBlockList();

  explicit pageBlockList(std::vector<object_ptr<pageBlockListItem>> &&items_);

  static const std::int32_t ID = -1037074852;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockBlockQuote final : public PageBlock {
 public:
  object_ptr<RichText> text_;
  object_ptr<RichText> credit_;

  pageBlockBlockQuote();

  pageBlockBlockQuote(object_ptr<RichText> &&text_, object_ptr<RichText> &&credit_);

  static const std::int32_t ID = 1657834142;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockPullQuote final : public PageBlock {
 public:
  object_ptr<RichText> text_;
  object_ptr<RichText> credit_;

  pageBlockPullQuote();

  pageBlockPullQuote(object_ptr<RichText> &&text_, object_ptr<RichText> &&credit_);

  static const std::int32_t ID = 490242317;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockAnimation final : public PageBlock {
 public:
  object_ptr<animation> animation_;
  object_ptr<pageBlockCaption> caption_;
  bool need_autoplay_;

  pageBlockAnimation();

  pageBlockAnimation(object_ptr<animation> &&animation_, object_ptr<pageBlockCaption> &&caption_, bool need_autoplay_);

  static const std::int32_t ID = 1355669513;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockAudio final : public PageBlock {
 public:
  object_ptr<audio> audio_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockAudio();

  pageBlockAudio(object_ptr<audio> &&audio_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = -63371245;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockPhoto final : public PageBlock {
 public:
  object_ptr<photo> photo_;
  object_ptr<pageBlockCaption> caption_;
  std::string url_;

  pageBlockPhoto();

  pageBlockPhoto(object_ptr<photo> &&photo_, object_ptr<pageBlockCaption> &&caption_, std::string const &url_);

  static const std::int32_t ID = 417601156;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockVideo final : public PageBlock {
 public:
  object_ptr<video> video_;
  object_ptr<pageBlockCaption> caption_;
  bool need_autoplay_;
  bool is_looped_;

  pageBlockVideo();

  pageBlockVideo(object_ptr<video> &&video_, object_ptr<pageBlockCaption> &&caption_, bool need_autoplay_, bool is_looped_);

  static const std::int32_t ID = 510041394;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockVoiceNote final : public PageBlock {
 public:
  object_ptr<voiceNote> voice_note_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockVoiceNote();

  pageBlockVoiceNote(object_ptr<voiceNote> &&voice_note_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = 1823310463;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockCover final : public PageBlock {
 public:
  object_ptr<PageBlock> cover_;

  pageBlockCover();

  explicit pageBlockCover(object_ptr<PageBlock> &&cover_);

  static const std::int32_t ID = 972174080;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockEmbedded final : public PageBlock {
 public:
  std::string url_;
  std::string html_;
  object_ptr<photo> poster_photo_;
  std::int32_t width_;
  std::int32_t height_;
  object_ptr<pageBlockCaption> caption_;
  bool is_full_width_;
  bool allow_scrolling_;

  pageBlockEmbedded();

  pageBlockEmbedded(std::string const &url_, std::string const &html_, object_ptr<photo> &&poster_photo_, std::int32_t width_, std::int32_t height_, object_ptr<pageBlockCaption> &&caption_, bool is_full_width_, bool allow_scrolling_);

  static const std::int32_t ID = -1942577763;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockEmbeddedPost final : public PageBlock {
 public:
  std::string url_;
  std::string author_;
  object_ptr<photo> author_photo_;
  std::int32_t date_;
  std::vector<object_ptr<PageBlock>> page_blocks_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockEmbeddedPost();

  pageBlockEmbeddedPost(std::string const &url_, std::string const &author_, object_ptr<photo> &&author_photo_, std::int32_t date_, std::vector<object_ptr<PageBlock>> &&page_blocks_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = 397600949;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockCollage final : public PageBlock {
 public:
  std::vector<object_ptr<PageBlock>> page_blocks_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockCollage();

  pageBlockCollage(std::vector<object_ptr<PageBlock>> &&page_blocks_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = 1163760110;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockSlideshow final : public PageBlock {
 public:
  std::vector<object_ptr<PageBlock>> page_blocks_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockSlideshow();

  pageBlockSlideshow(std::vector<object_ptr<PageBlock>> &&page_blocks_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = 539217375;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockChatLink final : public PageBlock {
 public:
  std::string title_;
  object_ptr<chatPhoto> photo_;
  std::string username_;

  pageBlockChatLink();

  pageBlockChatLink(std::string const &title_, object_ptr<chatPhoto> &&photo_, std::string const &username_);

  static const std::int32_t ID = 214606645;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockTable final : public PageBlock {
 public:
  object_ptr<RichText> caption_;
  std::vector<std::vector<object_ptr<pageBlockTableCell>>> cells_;
  bool is_bordered_;
  bool is_striped_;

  pageBlockTable();

  pageBlockTable(object_ptr<RichText> &&caption_, std::vector<std::vector<object_ptr<pageBlockTableCell>>> &&cells_, bool is_bordered_, bool is_striped_);

  static const std::int32_t ID = -942649288;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockDetails final : public PageBlock {
 public:
  object_ptr<RichText> header_;
  std::vector<object_ptr<PageBlock>> page_blocks_;
  bool is_open_;

  pageBlockDetails();

  pageBlockDetails(object_ptr<RichText> &&header_, std::vector<object_ptr<PageBlock>> &&page_blocks_, bool is_open_);

  static const std::int32_t ID = -1599869809;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockRelatedArticles final : public PageBlock {
 public:
  object_ptr<RichText> header_;
  std::vector<object_ptr<pageBlockRelatedArticle>> articles_;

  pageBlockRelatedArticles();

  pageBlockRelatedArticles(object_ptr<RichText> &&header_, std::vector<object_ptr<pageBlockRelatedArticle>> &&articles_);

  static const std::int32_t ID = -1807324374;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockMap final : public PageBlock {
 public:
  object_ptr<location> location_;
  std::int32_t zoom_;
  std::int32_t width_;
  std::int32_t height_;
  object_ptr<pageBlockCaption> caption_;

  pageBlockMap();

  pageBlockMap(object_ptr<location> &&location_, std::int32_t zoom_, std::int32_t width_, std::int32_t height_, object_ptr<pageBlockCaption> &&caption_);

  static const std::int32_t ID = 1510961171;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockCaption final : public Object {
 public:
  object_ptr<RichText> text_;
  object_ptr<RichText> credit_;

  pageBlockCaption();

  pageBlockCaption(object_ptr<RichText> &&text_, object_ptr<RichText> &&credit_);

  static const std::int32_t ID = -1180064650;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PageBlockHorizontalAlignment: public Object {
 public:
};

class pageBlockHorizontalAlignmentLeft final : public PageBlockHorizontalAlignment {
 public:

  pageBlockHorizontalAlignmentLeft();

  static const std::int32_t ID = 848701417;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockHorizontalAlignmentCenter final : public PageBlockHorizontalAlignment {
 public:

  pageBlockHorizontalAlignmentCenter();

  static const std::int32_t ID = -1009203990;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockHorizontalAlignmentRight final : public PageBlockHorizontalAlignment {
 public:

  pageBlockHorizontalAlignmentRight();

  static const std::int32_t ID = 1371369214;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockListItem final : public Object {
 public:
  std::string label_;
  std::vector<object_ptr<PageBlock>> page_blocks_;

  pageBlockListItem();

  pageBlockListItem(std::string const &label_, std::vector<object_ptr<PageBlock>> &&page_blocks_);

  static const std::int32_t ID = 323186259;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockRelatedArticle final : public Object {
 public:
  std::string url_;
  std::string title_;
  std::string description_;
  object_ptr<photo> photo_;
  std::string author_;
  std::int32_t publish_date_;

  pageBlockRelatedArticle();

  pageBlockRelatedArticle(std::string const &url_, std::string const &title_, std::string const &description_, object_ptr<photo> &&photo_, std::string const &author_, std::int32_t publish_date_);

  static const std::int32_t ID = 481199251;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockTableCell final : public Object {
 public:
  object_ptr<RichText> text_;
  bool is_header_;
  std::int32_t colspan_;
  std::int32_t rowspan_;
  object_ptr<PageBlockHorizontalAlignment> align_;
  object_ptr<PageBlockVerticalAlignment> valign_;

  pageBlockTableCell();

  pageBlockTableCell(object_ptr<RichText> &&text_, bool is_header_, std::int32_t colspan_, std::int32_t rowspan_, object_ptr<PageBlockHorizontalAlignment> &&align_, object_ptr<PageBlockVerticalAlignment> &&valign_);

  static const std::int32_t ID = 1417658214;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PageBlockVerticalAlignment: public Object {
 public:
};

class pageBlockVerticalAlignmentTop final : public PageBlockVerticalAlignment {
 public:

  pageBlockVerticalAlignmentTop();

  static const std::int32_t ID = 195500454;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockVerticalAlignmentMiddle final : public PageBlockVerticalAlignment {
 public:

  pageBlockVerticalAlignmentMiddle();

  static const std::int32_t ID = -2123096587;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pageBlockVerticalAlignmentBottom final : public PageBlockVerticalAlignment {
 public:

  pageBlockVerticalAlignmentBottom();

  static const std::int32_t ID = 2092531158;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportAuthorizationForm final : public Object {
 public:
  std::int32_t id_;
  std::vector<object_ptr<passportRequiredElement>> required_elements_;
  std::string privacy_policy_url_;

  passportAuthorizationForm();

  passportAuthorizationForm(std::int32_t id_, std::vector<object_ptr<passportRequiredElement>> &&required_elements_, std::string const &privacy_policy_url_);

  static const std::int32_t ID = -1070673218;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PassportElement: public Object {
 public:
};

class passportElementPersonalDetails final : public PassportElement {
 public:
  object_ptr<personalDetails> personal_details_;

  passportElementPersonalDetails();

  explicit passportElementPersonalDetails(object_ptr<personalDetails> &&personal_details_);

  static const std::int32_t ID = 1217724035;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementPassport final : public PassportElement {
 public:
  object_ptr<identityDocument> passport_;

  passportElementPassport();

  explicit passportElementPassport(object_ptr<identityDocument> &&passport_);

  static const std::int32_t ID = -263985373;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementDriverLicense final : public PassportElement {
 public:
  object_ptr<identityDocument> driver_license_;

  passportElementDriverLicense();

  explicit passportElementDriverLicense(object_ptr<identityDocument> &&driver_license_);

  static const std::int32_t ID = 1643580589;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementIdentityCard final : public PassportElement {
 public:
  object_ptr<identityDocument> identity_card_;

  passportElementIdentityCard();

  explicit passportElementIdentityCard(object_ptr<identityDocument> &&identity_card_);

  static const std::int32_t ID = 2083775797;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementInternalPassport final : public PassportElement {
 public:
  object_ptr<identityDocument> internal_passport_;

  passportElementInternalPassport();

  explicit passportElementInternalPassport(object_ptr<identityDocument> &&internal_passport_);

  static const std::int32_t ID = 36220295;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementAddress final : public PassportElement {
 public:
  object_ptr<address> address_;

  passportElementAddress();

  explicit passportElementAddress(object_ptr<address> &&address_);

  static const std::int32_t ID = -782625232;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementUtilityBill final : public PassportElement {
 public:
  object_ptr<personalDocument> utility_bill_;

  passportElementUtilityBill();

  explicit passportElementUtilityBill(object_ptr<personalDocument> &&utility_bill_);

  static const std::int32_t ID = -234611246;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementBankStatement final : public PassportElement {
 public:
  object_ptr<personalDocument> bank_statement_;

  passportElementBankStatement();

  explicit passportElementBankStatement(object_ptr<personalDocument> &&bank_statement_);

  static const std::int32_t ID = -366464408;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementRentalAgreement final : public PassportElement {
 public:
  object_ptr<personalDocument> rental_agreement_;

  passportElementRentalAgreement();

  explicit passportElementRentalAgreement(object_ptr<personalDocument> &&rental_agreement_);

  static const std::int32_t ID = -290141400;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementPassportRegistration final : public PassportElement {
 public:
  object_ptr<personalDocument> passport_registration_;

  passportElementPassportRegistration();

  explicit passportElementPassportRegistration(object_ptr<personalDocument> &&passport_registration_);

  static const std::int32_t ID = 618323071;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTemporaryRegistration final : public PassportElement {
 public:
  object_ptr<personalDocument> temporary_registration_;

  passportElementTemporaryRegistration();

  explicit passportElementTemporaryRegistration(object_ptr<personalDocument> &&temporary_registration_);

  static const std::int32_t ID = 1237626864;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementPhoneNumber final : public PassportElement {
 public:
  std::string phone_number_;

  passportElementPhoneNumber();

  explicit passportElementPhoneNumber(std::string const &phone_number_);

  static const std::int32_t ID = -1320118375;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementEmailAddress final : public PassportElement {
 public:
  std::string email_address_;

  passportElementEmailAddress();

  explicit passportElementEmailAddress(std::string const &email_address_);

  static const std::int32_t ID = -1528129531;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementError final : public Object {
 public:
  object_ptr<PassportElementType> type_;
  std::string message_;
  object_ptr<PassportElementErrorSource> source_;

  passportElementError();

  passportElementError(object_ptr<PassportElementType> &&type_, std::string const &message_, object_ptr<PassportElementErrorSource> &&source_);

  static const std::int32_t ID = -1861902395;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PassportElementErrorSource: public Object {
 public:
};

class passportElementErrorSourceUnspecified final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceUnspecified();

  static const std::int32_t ID = -378320830;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceDataField final : public PassportElementErrorSource {
 public:
  std::string field_name_;

  passportElementErrorSourceDataField();

  explicit passportElementErrorSourceDataField(std::string const &field_name_);

  static const std::int32_t ID = -308650776;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceFrontSide final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceFrontSide();

  static const std::int32_t ID = 1895658292;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceReverseSide final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceReverseSide();

  static const std::int32_t ID = 1918630391;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceSelfie final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceSelfie();

  static const std::int32_t ID = -797043672;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceTranslationFile final : public PassportElementErrorSource {
 public:
  std::int32_t file_index_;

  passportElementErrorSourceTranslationFile();

  explicit passportElementErrorSourceTranslationFile(std::int32_t file_index_);

  static const std::int32_t ID = -689621228;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceTranslationFiles final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceTranslationFiles();

  static const std::int32_t ID = 581280796;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceFile final : public PassportElementErrorSource {
 public:
  std::int32_t file_index_;

  passportElementErrorSourceFile();

  explicit passportElementErrorSourceFile(std::int32_t file_index_);

  static const std::int32_t ID = 2020358960;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementErrorSourceFiles final : public PassportElementErrorSource {
 public:

  passportElementErrorSourceFiles();

  static const std::int32_t ID = 1894164178;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PassportElementType: public Object {
 public:
};

class passportElementTypePersonalDetails final : public PassportElementType {
 public:

  passportElementTypePersonalDetails();

  static const std::int32_t ID = -1032136365;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypePassport final : public PassportElementType {
 public:

  passportElementTypePassport();

  static const std::int32_t ID = -436360376;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeDriverLicense final : public PassportElementType {
 public:

  passportElementTypeDriverLicense();

  static const std::int32_t ID = 1827298379;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeIdentityCard final : public PassportElementType {
 public:

  passportElementTypeIdentityCard();

  static const std::int32_t ID = -502356132;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeInternalPassport final : public PassportElementType {
 public:

  passportElementTypeInternalPassport();

  static const std::int32_t ID = -793781959;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeAddress final : public PassportElementType {
 public:

  passportElementTypeAddress();

  static const std::int32_t ID = 496327874;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeUtilityBill final : public PassportElementType {
 public:

  passportElementTypeUtilityBill();

  static const std::int32_t ID = 627084906;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeBankStatement final : public PassportElementType {
 public:

  passportElementTypeBankStatement();

  static const std::int32_t ID = 574095667;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeRentalAgreement final : public PassportElementType {
 public:

  passportElementTypeRentalAgreement();

  static const std::int32_t ID = -2060583280;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypePassportRegistration final : public PassportElementType {
 public:

  passportElementTypePassportRegistration();

  static const std::int32_t ID = -159478209;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeTemporaryRegistration final : public PassportElementType {
 public:

  passportElementTypeTemporaryRegistration();

  static const std::int32_t ID = 1092498527;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypePhoneNumber final : public PassportElementType {
 public:

  passportElementTypePhoneNumber();

  static const std::int32_t ID = -995361172;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementTypeEmailAddress final : public PassportElementType {
 public:

  passportElementTypeEmailAddress();

  static const std::int32_t ID = -79321405;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElements final : public Object {
 public:
  std::vector<object_ptr<PassportElement>> elements_;

  passportElements();

  explicit passportElements(std::vector<object_ptr<PassportElement>> &&elements_);

  static const std::int32_t ID = 1264617556;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportElementsWithErrors final : public Object {
 public:
  std::vector<object_ptr<PassportElement>> elements_;
  std::vector<object_ptr<passportElementError>> errors_;

  passportElementsWithErrors();

  passportElementsWithErrors(std::vector<object_ptr<PassportElement>> &&elements_, std::vector<object_ptr<passportElementError>> &&errors_);

  static const std::int32_t ID = 1308923044;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportRequiredElement final : public Object {
 public:
  std::vector<object_ptr<passportSuitableElement>> suitable_elements_;

  passportRequiredElement();

  explicit passportRequiredElement(std::vector<object_ptr<passportSuitableElement>> &&suitable_elements_);

  static const std::int32_t ID = -1983641651;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passportSuitableElement final : public Object {
 public:
  object_ptr<PassportElementType> type_;
  bool is_selfie_required_;
  bool is_translation_required_;
  bool is_native_name_required_;

  passportSuitableElement();

  passportSuitableElement(object_ptr<PassportElementType> &&type_, bool is_selfie_required_, bool is_translation_required_, bool is_native_name_required_);

  static const std::int32_t ID = -789019876;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class passwordState final : public Object {
 public:
  bool has_password_;
  std::string password_hint_;
  bool has_recovery_email_address_;
  bool has_passport_data_;
  object_ptr<emailAddressAuthenticationCodeInfo> recovery_email_address_code_info_;

  passwordState();

  passwordState(bool has_password_, std::string const &password_hint_, bool has_recovery_email_address_, bool has_passport_data_, object_ptr<emailAddressAuthenticationCodeInfo> &&recovery_email_address_code_info_);

  static const std::int32_t ID = -1154797731;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class paymentForm final : public Object {
 public:
  object_ptr<invoice> invoice_;
  std::string url_;
  object_ptr<paymentsProviderStripe> payments_provider_;
  object_ptr<orderInfo> saved_order_info_;
  object_ptr<savedCredentials> saved_credentials_;
  bool can_save_credentials_;
  bool need_password_;

  paymentForm();

  paymentForm(object_ptr<invoice> &&invoice_, std::string const &url_, object_ptr<paymentsProviderStripe> &&payments_provider_, object_ptr<orderInfo> &&saved_order_info_, object_ptr<savedCredentials> &&saved_credentials_, bool can_save_credentials_, bool need_password_);

  static const std::int32_t ID = -200418230;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class paymentReceipt final : public Object {
 public:
  std::int32_t date_;
  std::int32_t payments_provider_user_id_;
  object_ptr<invoice> invoice_;
  object_ptr<orderInfo> order_info_;
  object_ptr<shippingOption> shipping_option_;
  std::string credentials_title_;

  paymentReceipt();

  paymentReceipt(std::int32_t date_, std::int32_t payments_provider_user_id_, object_ptr<invoice> &&invoice_, object_ptr<orderInfo> &&order_info_, object_ptr<shippingOption> &&shipping_option_, std::string const &credentials_title_);

  static const std::int32_t ID = -1171223545;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class paymentResult final : public Object {
 public:
  bool success_;
  std::string verification_url_;

  paymentResult();

  paymentResult(bool success_, std::string const &verification_url_);

  static const std::int32_t ID = -804263843;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class paymentsProviderStripe final : public Object {
 public:
  std::string publishable_key_;
  bool need_country_;
  bool need_postal_code_;
  bool need_cardholder_name_;

  paymentsProviderStripe();

  paymentsProviderStripe(std::string const &publishable_key_, bool need_country_, bool need_postal_code_, bool need_cardholder_name_);

  static const std::int32_t ID = 1090791032;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class personalDetails final : public Object {
 public:
  std::string first_name_;
  std::string middle_name_;
  std::string last_name_;
  std::string native_first_name_;
  std::string native_middle_name_;
  std::string native_last_name_;
  object_ptr<date> birthdate_;
  std::string gender_;
  std::string country_code_;
  std::string residence_country_code_;

  personalDetails();

  personalDetails(std::string const &first_name_, std::string const &middle_name_, std::string const &last_name_, std::string const &native_first_name_, std::string const &native_middle_name_, std::string const &native_last_name_, object_ptr<date> &&birthdate_, std::string const &gender_, std::string const &country_code_, std::string const &residence_country_code_);

  static const std::int32_t ID = -1061656137;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class personalDocument final : public Object {
 public:
  std::vector<object_ptr<datedFile>> files_;
  std::vector<object_ptr<datedFile>> translation_;

  personalDocument();

  personalDocument(std::vector<object_ptr<datedFile>> &&files_, std::vector<object_ptr<datedFile>> &&translation_);

  static const std::int32_t ID = -1011634661;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class phoneNumberAuthenticationSettings final : public Object {
 public:
  bool allow_flash_call_;
  bool is_current_phone_number_;
  bool allow_sms_retriever_api_;

  phoneNumberAuthenticationSettings();

  phoneNumberAuthenticationSettings(bool allow_flash_call_, bool is_current_phone_number_, bool allow_sms_retriever_api_);

  static const std::int32_t ID = -859198743;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class photo final : public Object {
 public:
  bool has_stickers_;
  object_ptr<minithumbnail> minithumbnail_;
  std::vector<object_ptr<photoSize>> sizes_;

  photo();

  photo(bool has_stickers_, object_ptr<minithumbnail> &&minithumbnail_, std::vector<object_ptr<photoSize>> &&sizes_);

  static const std::int32_t ID = -2022871583;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class photoSize final : public Object {
 public:
  std::string type_;
  object_ptr<file> photo_;
  std::int32_t width_;
  std::int32_t height_;

  photoSize();

  photoSize(std::string const &type_, object_ptr<file> &&photo_, std::int32_t width_, std::int32_t height_);

  static const std::int32_t ID = 421980227;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class poll final : public Object {
 public:
  std::int64_t id_;
  std::string question_;
  std::vector<object_ptr<pollOption>> options_;
  std::int32_t total_voter_count_;
  std::vector<std::int32_t> recent_voter_user_ids_;
  bool is_anonymous_;
  object_ptr<PollType> type_;
  bool is_closed_;

  poll();

  poll(std::int64_t id_, std::string const &question_, std::vector<object_ptr<pollOption>> &&options_, std::int32_t total_voter_count_, std::vector<std::int32_t> &&recent_voter_user_ids_, bool is_anonymous_, object_ptr<PollType> &&type_, bool is_closed_);

  static const std::int32_t ID = -964385924;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pollOption final : public Object {
 public:
  std::string text_;
  std::int32_t voter_count_;
  std::int32_t vote_percentage_;
  bool is_chosen_;
  bool is_being_chosen_;

  pollOption();

  pollOption(std::string const &text_, std::int32_t voter_count_, std::int32_t vote_percentage_, bool is_chosen_, bool is_being_chosen_);

  static const std::int32_t ID = 1473893797;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PollType: public Object {
 public:
};

class pollTypeRegular final : public PollType {
 public:
  bool allow_multiple_answers_;

  pollTypeRegular();

  explicit pollTypeRegular(bool allow_multiple_answers_);

  static const std::int32_t ID = 641265698;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pollTypeQuiz final : public PollType {
 public:
  std::int32_t correct_option_id_;

  pollTypeQuiz();

  explicit pollTypeQuiz(std::int32_t correct_option_id_);

  static const std::int32_t ID = -354461930;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class profilePhoto final : public Object {
 public:
  std::int64_t id_;
  object_ptr<file> small_;
  object_ptr<file> big_;

  profilePhoto();

  profilePhoto(std::int64_t id_, object_ptr<file> &&small_, object_ptr<file> &&big_);

  static const std::int32_t ID = 978085937;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class proxies final : public Object {
 public:
  std::vector<object_ptr<proxy>> proxies_;

  proxies();

  explicit proxies(std::vector<object_ptr<proxy>> &&proxies_);

  static const std::int32_t ID = 1200447205;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class proxy final : public Object {
 public:
  std::int32_t id_;
  std::string server_;
  std::int32_t port_;
  std::int32_t last_used_date_;
  bool is_enabled_;
  object_ptr<ProxyType> type_;

  proxy();

  proxy(std::int32_t id_, std::string const &server_, std::int32_t port_, std::int32_t last_used_date_, bool is_enabled_, object_ptr<ProxyType> &&type_);

  static const std::int32_t ID = 196049779;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ProxyType: public Object {
 public:
};

class proxyTypeSocks5 final : public ProxyType {
 public:
  std::string username_;
  std::string password_;

  proxyTypeSocks5();

  proxyTypeSocks5(std::string const &username_, std::string const &password_);

  static const std::int32_t ID = -890027341;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class proxyTypeHttp final : public ProxyType {
 public:
  std::string username_;
  std::string password_;
  bool http_only_;

  proxyTypeHttp();

  proxyTypeHttp(std::string const &username_, std::string const &password_, bool http_only_);

  static const std::int32_t ID = -1547188361;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class proxyTypeMtproto final : public ProxyType {
 public:
  std::string secret_;

  proxyTypeMtproto();

  explicit proxyTypeMtproto(std::string const &secret_);

  static const std::int32_t ID = -1964826627;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PublicChatType: public Object {
 public:
};

class publicChatTypeHasUsername final : public PublicChatType {
 public:

  publicChatTypeHasUsername();

  static const std::int32_t ID = 350789758;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class publicChatTypeIsLocationBased final : public PublicChatType {
 public:

  publicChatTypeIsLocationBased();

  static const std::int32_t ID = 1183735952;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class publicMessageLink final : public Object {
 public:
  std::string link_;
  std::string html_;

  publicMessageLink();

  publicMessageLink(std::string const &link_, std::string const &html_);

  static const std::int32_t ID = -679603433;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class PushMessageContent: public Object {
 public:
};

class pushMessageContentHidden final : public PushMessageContent {
 public:
  bool is_pinned_;

  pushMessageContentHidden();

  explicit pushMessageContentHidden(bool is_pinned_);

  static const std::int32_t ID = -316950436;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentAnimation final : public PushMessageContent {
 public:
  object_ptr<animation> animation_;
  std::string caption_;
  bool is_pinned_;

  pushMessageContentAnimation();

  pushMessageContentAnimation(object_ptr<animation> &&animation_, std::string const &caption_, bool is_pinned_);

  static const std::int32_t ID = 1034215396;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentAudio final : public PushMessageContent {
 public:
  object_ptr<audio> audio_;
  bool is_pinned_;

  pushMessageContentAudio();

  pushMessageContentAudio(object_ptr<audio> &&audio_, bool is_pinned_);

  static const std::int32_t ID = 381581426;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentContact final : public PushMessageContent {
 public:
  std::string name_;
  bool is_pinned_;

  pushMessageContentContact();

  pushMessageContentContact(std::string const &name_, bool is_pinned_);

  static const std::int32_t ID = -12219820;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentContactRegistered final : public PushMessageContent {
 public:

  pushMessageContentContactRegistered();

  static const std::int32_t ID = -303962720;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentDocument final : public PushMessageContent {
 public:
  object_ptr<document> document_;
  bool is_pinned_;

  pushMessageContentDocument();

  pushMessageContentDocument(object_ptr<document> &&document_, bool is_pinned_);

  static const std::int32_t ID = -458379775;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentGame final : public PushMessageContent {
 public:
  std::string title_;
  bool is_pinned_;

  pushMessageContentGame();

  pushMessageContentGame(std::string const &title_, bool is_pinned_);

  static const std::int32_t ID = -515131109;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentGameScore final : public PushMessageContent {
 public:
  std::string title_;
  std::int32_t score_;
  bool is_pinned_;

  pushMessageContentGameScore();

  pushMessageContentGameScore(std::string const &title_, std::int32_t score_, bool is_pinned_);

  static const std::int32_t ID = 901303688;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentInvoice final : public PushMessageContent {
 public:
  std::string price_;
  bool is_pinned_;

  pushMessageContentInvoice();

  pushMessageContentInvoice(std::string const &price_, bool is_pinned_);

  static const std::int32_t ID = -1731687492;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentLocation final : public PushMessageContent {
 public:
  bool is_live_;
  bool is_pinned_;

  pushMessageContentLocation();

  pushMessageContentLocation(bool is_live_, bool is_pinned_);

  static const std::int32_t ID = -1288005709;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentPhoto final : public PushMessageContent {
 public:
  object_ptr<photo> photo_;
  std::string caption_;
  bool is_secret_;
  bool is_pinned_;

  pushMessageContentPhoto();

  pushMessageContentPhoto(object_ptr<photo> &&photo_, std::string const &caption_, bool is_secret_, bool is_pinned_);

  static const std::int32_t ID = 140631122;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentPoll final : public PushMessageContent {
 public:
  std::string question_;
  bool is_regular_;
  bool is_pinned_;

  pushMessageContentPoll();

  pushMessageContentPoll(std::string const &question_, bool is_regular_, bool is_pinned_);

  static const std::int32_t ID = -44403654;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentScreenshotTaken final : public PushMessageContent {
 public:

  pushMessageContentScreenshotTaken();

  static const std::int32_t ID = 214245369;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentSticker final : public PushMessageContent {
 public:
  object_ptr<sticker> sticker_;
  std::string emoji_;
  bool is_pinned_;

  pushMessageContentSticker();

  pushMessageContentSticker(object_ptr<sticker> &&sticker_, std::string const &emoji_, bool is_pinned_);

  static const std::int32_t ID = 1553513939;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentText final : public PushMessageContent {
 public:
  std::string text_;
  bool is_pinned_;

  pushMessageContentText();

  pushMessageContentText(std::string const &text_, bool is_pinned_);

  static const std::int32_t ID = 274587305;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentVideo final : public PushMessageContent {
 public:
  object_ptr<video> video_;
  std::string caption_;
  bool is_secret_;
  bool is_pinned_;

  pushMessageContentVideo();

  pushMessageContentVideo(object_ptr<video> &&video_, std::string const &caption_, bool is_secret_, bool is_pinned_);

  static const std::int32_t ID = 310038831;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentVideoNote final : public PushMessageContent {
 public:
  object_ptr<videoNote> video_note_;
  bool is_pinned_;

  pushMessageContentVideoNote();

  pushMessageContentVideoNote(object_ptr<videoNote> &&video_note_, bool is_pinned_);

  static const std::int32_t ID = -1122764417;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentVoiceNote final : public PushMessageContent {
 public:
  object_ptr<voiceNote> voice_note_;
  bool is_pinned_;

  pushMessageContentVoiceNote();

  pushMessageContentVoiceNote(object_ptr<voiceNote> &&voice_note_, bool is_pinned_);

  static const std::int32_t ID = 88910987;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentBasicGroupChatCreate final : public PushMessageContent {
 public:

  pushMessageContentBasicGroupChatCreate();

  static const std::int32_t ID = -2114855172;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentChatAddMembers final : public PushMessageContent {
 public:
  std::string member_name_;
  bool is_current_user_;
  bool is_returned_;

  pushMessageContentChatAddMembers();

  pushMessageContentChatAddMembers(std::string const &member_name_, bool is_current_user_, bool is_returned_);

  static const std::int32_t ID = -1087145158;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentChatChangePhoto final : public PushMessageContent {
 public:

  pushMessageContentChatChangePhoto();

  static const std::int32_t ID = -1114222051;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentChatChangeTitle final : public PushMessageContent {
 public:
  std::string title_;

  pushMessageContentChatChangeTitle();

  explicit pushMessageContentChatChangeTitle(std::string const &title_);

  static const std::int32_t ID = -1964902749;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentChatDeleteMember final : public PushMessageContent {
 public:
  std::string member_name_;
  bool is_current_user_;
  bool is_left_;

  pushMessageContentChatDeleteMember();

  pushMessageContentChatDeleteMember(std::string const &member_name_, bool is_current_user_, bool is_left_);

  static const std::int32_t ID = 598714783;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentChatJoinByLink final : public PushMessageContent {
 public:

  pushMessageContentChatJoinByLink();

  static const std::int32_t ID = 1553719113;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentMessageForwards final : public PushMessageContent {
 public:
  std::int32_t total_count_;

  pushMessageContentMessageForwards();

  explicit pushMessageContentMessageForwards(std::int32_t total_count_);

  static const std::int32_t ID = -1913083876;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushMessageContentMediaAlbum final : public PushMessageContent {
 public:
  std::int32_t total_count_;
  bool has_photos_;
  bool has_videos_;

  pushMessageContentMediaAlbum();

  pushMessageContentMediaAlbum(std::int32_t total_count_, bool has_photos_, bool has_videos_);

  static const std::int32_t ID = -874278109;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pushReceiverId final : public Object {
 public:
  std::int64_t id_;

  pushReceiverId();

  explicit pushReceiverId(std::int64_t id_);

  static const std::int32_t ID = 371056428;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class recoveryEmailAddress final : public Object {
 public:
  std::string recovery_email_address_;

  recoveryEmailAddress();

  explicit recoveryEmailAddress(std::string const &recovery_email_address_);

  static const std::int32_t ID = 1290526187;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class remoteFile final : public Object {
 public:
  std::string id_;
  std::string unique_id_;
  bool is_uploading_active_;
  bool is_uploading_completed_;
  std::int32_t uploaded_size_;

  remoteFile();

  remoteFile(std::string const &id_, std::string const &unique_id_, bool is_uploading_active_, bool is_uploading_completed_, std::int32_t uploaded_size_);

  static const std::int32_t ID = -1822143022;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class ReplyMarkup: public Object {
 public:
};

class replyMarkupRemoveKeyboard final : public ReplyMarkup {
 public:
  bool is_personal_;

  replyMarkupRemoveKeyboard();

  explicit replyMarkupRemoveKeyboard(bool is_personal_);

  static const std::int32_t ID = -691252879;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class replyMarkupForceReply final : public ReplyMarkup {
 public:
  bool is_personal_;

  replyMarkupForceReply();

  explicit replyMarkupForceReply(bool is_personal_);

  static const std::int32_t ID = 1039104593;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class replyMarkupShowKeyboard final : public ReplyMarkup {
 public:
  std::vector<std::vector<object_ptr<keyboardButton>>> rows_;
  bool resize_keyboard_;
  bool one_time_;
  bool is_personal_;

  replyMarkupShowKeyboard();

  replyMarkupShowKeyboard(std::vector<std::vector<object_ptr<keyboardButton>>> &&rows_, bool resize_keyboard_, bool one_time_, bool is_personal_);

  static const std::int32_t ID = -992627133;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class replyMarkupInlineKeyboard final : public ReplyMarkup {
 public:
  std::vector<std::vector<object_ptr<inlineKeyboardButton>>> rows_;

  replyMarkupInlineKeyboard();

  explicit replyMarkupInlineKeyboard(std::vector<std::vector<object_ptr<inlineKeyboardButton>>> &&rows_);

  static const std::int32_t ID = -619317658;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class RichText: public Object {
 public:
};

class richTextPlain final : public RichText {
 public:
  std::string text_;

  richTextPlain();

  explicit richTextPlain(std::string const &text_);

  static const std::int32_t ID = 482617702;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextBold final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextBold();

  explicit richTextBold(object_ptr<RichText> &&text_);

  static const std::int32_t ID = 1670844268;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextItalic final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextItalic();

  explicit richTextItalic(object_ptr<RichText> &&text_);

  static const std::int32_t ID = 1853354047;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextUnderline final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextUnderline();

  explicit richTextUnderline(object_ptr<RichText> &&text_);

  static const std::int32_t ID = -536019572;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextStrikethrough final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextStrikethrough();

  explicit richTextStrikethrough(object_ptr<RichText> &&text_);

  static const std::int32_t ID = 723413585;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextFixed final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextFixed();

  explicit richTextFixed(object_ptr<RichText> &&text_);

  static const std::int32_t ID = -1271496249;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextUrl final : public RichText {
 public:
  object_ptr<RichText> text_;
  std::string url_;
  bool is_cached_;

  richTextUrl();

  richTextUrl(object_ptr<RichText> &&text_, std::string const &url_, bool is_cached_);

  static const std::int32_t ID = 83939092;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextEmailAddress final : public RichText {
 public:
  object_ptr<RichText> text_;
  std::string email_address_;

  richTextEmailAddress();

  richTextEmailAddress(object_ptr<RichText> &&text_, std::string const &email_address_);

  static const std::int32_t ID = 40018679;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextSubscript final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextSubscript();

  explicit richTextSubscript(object_ptr<RichText> &&text_);

  static const std::int32_t ID = -868197812;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextSuperscript final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextSuperscript();

  explicit richTextSuperscript(object_ptr<RichText> &&text_);

  static const std::int32_t ID = -382241437;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextMarked final : public RichText {
 public:
  object_ptr<RichText> text_;

  richTextMarked();

  explicit richTextMarked(object_ptr<RichText> &&text_);

  static const std::int32_t ID = -1271999614;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextPhoneNumber final : public RichText {
 public:
  object_ptr<RichText> text_;
  std::string phone_number_;

  richTextPhoneNumber();

  richTextPhoneNumber(object_ptr<RichText> &&text_, std::string const &phone_number_);

  static const std::int32_t ID = 128521539;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextIcon final : public RichText {
 public:
  object_ptr<document> document_;
  std::int32_t width_;
  std::int32_t height_;

  richTextIcon();

  richTextIcon(object_ptr<document> &&document_, std::int32_t width_, std::int32_t height_);

  static const std::int32_t ID = -1480316158;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTextAnchor final : public RichText {
 public:
  object_ptr<RichText> text_;
  std::string name_;

  richTextAnchor();

  richTextAnchor(object_ptr<RichText> &&text_, std::string const &name_);

  static const std::int32_t ID = 673137292;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class richTexts final : public RichText {
 public:
  std::vector<object_ptr<RichText>> texts_;

  richTexts();

  explicit richTexts(std::vector<object_ptr<RichText>> &&texts_);

  static const std::int32_t ID = 1647457821;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class savedCredentials final : public Object {
 public:
  std::string id_;
  std::string title_;

  savedCredentials();

  savedCredentials(std::string const &id_, std::string const &title_);

  static const std::int32_t ID = -370273060;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class scopeNotificationSettings final : public Object {
 public:
  std::int32_t mute_for_;
  std::string sound_;
  bool show_preview_;
  bool disable_pinned_message_notifications_;
  bool disable_mention_notifications_;

  scopeNotificationSettings();

  scopeNotificationSettings(std::int32_t mute_for_, std::string const &sound_, bool show_preview_, bool disable_pinned_message_notifications_, bool disable_mention_notifications_);

  static const std::int32_t ID = -426103745;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class SearchMessagesFilter: public Object {
 public:
};

class searchMessagesFilterEmpty final : public SearchMessagesFilter {
 public:

  searchMessagesFilterEmpty();

  static const std::int32_t ID = -869395657;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterAnimation final : public SearchMessagesFilter {
 public:

  searchMessagesFilterAnimation();

  static const std::int32_t ID = -155713339;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterAudio final : public SearchMessagesFilter {
 public:

  searchMessagesFilterAudio();

  static const std::int32_t ID = 867505275;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterDocument final : public SearchMessagesFilter {
 public:

  searchMessagesFilterDocument();

  static const std::int32_t ID = 1526331215;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterPhoto final : public SearchMessagesFilter {
 public:

  searchMessagesFilterPhoto();

  static const std::int32_t ID = 925932293;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterVideo final : public SearchMessagesFilter {
 public:

  searchMessagesFilterVideo();

  static const std::int32_t ID = 115538222;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterVoiceNote final : public SearchMessagesFilter {
 public:

  searchMessagesFilterVoiceNote();

  static const std::int32_t ID = 1841439357;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterPhotoAndVideo final : public SearchMessagesFilter {
 public:

  searchMessagesFilterPhotoAndVideo();

  static const std::int32_t ID = 1352130963;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterUrl final : public SearchMessagesFilter {
 public:

  searchMessagesFilterUrl();

  static const std::int32_t ID = -1828724341;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterChatPhoto final : public SearchMessagesFilter {
 public:

  searchMessagesFilterChatPhoto();

  static const std::int32_t ID = -1247751329;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterCall final : public SearchMessagesFilter {
 public:

  searchMessagesFilterCall();

  static const std::int32_t ID = 1305231012;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterMissedCall final : public SearchMessagesFilter {
 public:

  searchMessagesFilterMissedCall();

  static const std::int32_t ID = 970663098;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterVideoNote final : public SearchMessagesFilter {
 public:

  searchMessagesFilterVideoNote();

  static const std::int32_t ID = 564323321;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterVoiceAndVideoNote final : public SearchMessagesFilter {
 public:

  searchMessagesFilterVoiceAndVideoNote();

  static const std::int32_t ID = 664174819;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterMention final : public SearchMessagesFilter {
 public:

  searchMessagesFilterMention();

  static const std::int32_t ID = 2001258652;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessagesFilterUnreadMention final : public SearchMessagesFilter {
 public:

  searchMessagesFilterUnreadMention();

  static const std::int32_t ID = -95769149;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class seconds final : public Object {
 public:
  double seconds_;

  seconds();

  explicit seconds(double seconds_);

  static const std::int32_t ID = 959899022;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class secretChat final : public Object {
 public:
  std::int32_t id_;
  std::int32_t user_id_;
  object_ptr<SecretChatState> state_;
  bool is_outbound_;
  std::int32_t ttl_;
  std::string key_hash_;
  std::int32_t layer_;

  secretChat();

  secretChat(std::int32_t id_, std::int32_t user_id_, object_ptr<SecretChatState> &&state_, bool is_outbound_, std::int32_t ttl_, std::string const &key_hash_, std::int32_t layer_);

  static const std::int32_t ID = 1279231629;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class SecretChatState: public Object {
 public:
};

class secretChatStatePending final : public SecretChatState {
 public:

  secretChatStatePending();

  static const std::int32_t ID = -1637050756;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class secretChatStateReady final : public SecretChatState {
 public:

  secretChatStateReady();

  static const std::int32_t ID = -1611352087;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class secretChatStateClosed final : public SecretChatState {
 public:

  secretChatStateClosed();

  static const std::int32_t ID = -1945106707;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendMessageOptions final : public Object {
 public:
  bool disable_notification_;
  bool from_background_;
  object_ptr<MessageSchedulingState> scheduling_state_;

  sendMessageOptions();

  sendMessageOptions(bool disable_notification_, bool from_background_, object_ptr<MessageSchedulingState> &&scheduling_state_);

  static const std::int32_t ID = 669760254;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class session final : public Object {
 public:
  std::int64_t id_;
  bool is_current_;
  bool is_password_pending_;
  std::int32_t api_id_;
  std::string application_name_;
  std::string application_version_;
  bool is_official_application_;
  std::string device_model_;
  std::string platform_;
  std::string system_version_;
  std::int32_t log_in_date_;
  std::int32_t last_active_date_;
  std::string ip_;
  std::string country_;
  std::string region_;

  session();

  session(std::int64_t id_, bool is_current_, bool is_password_pending_, std::int32_t api_id_, std::string const &application_name_, std::string const &application_version_, bool is_official_application_, std::string const &device_model_, std::string const &platform_, std::string const &system_version_, std::int32_t log_in_date_, std::int32_t last_active_date_, std::string const &ip_, std::string const &country_, std::string const &region_);

  static const std::int32_t ID = 1920553176;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sessions final : public Object {
 public:
  std::vector<object_ptr<session>> sessions_;

  sessions();

  explicit sessions(std::vector<object_ptr<session>> &&sessions_);

  static const std::int32_t ID = -463118121;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class shippingOption final : public Object {
 public:
  std::string id_;
  std::string title_;
  std::vector<object_ptr<labeledPricePart>> price_parts_;

  shippingOption();

  shippingOption(std::string const &id_, std::string const &title_, std::vector<object_ptr<labeledPricePart>> &&price_parts_);

  static const std::int32_t ID = 1425690001;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sticker final : public Object {
 public:
  std::int64_t set_id_;
  std::int32_t width_;
  std::int32_t height_;
  std::string emoji_;
  bool is_animated_;
  bool is_mask_;
  object_ptr<maskPosition> mask_position_;
  object_ptr<photoSize> thumbnail_;
  object_ptr<file> sticker_;

  sticker();

  sticker(std::int64_t set_id_, std::int32_t width_, std::int32_t height_, std::string const &emoji_, bool is_animated_, bool is_mask_, object_ptr<maskPosition> &&mask_position_, object_ptr<photoSize> &&thumbnail_, object_ptr<file> &&sticker_);

  static const std::int32_t ID = -1835470627;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class stickerSet final : public Object {
 public:
  std::int64_t id_;
  std::string title_;
  std::string name_;
  object_ptr<photoSize> thumbnail_;
  bool is_installed_;
  bool is_archived_;
  bool is_official_;
  bool is_animated_;
  bool is_masks_;
  bool is_viewed_;
  std::vector<object_ptr<sticker>> stickers_;
  std::vector<object_ptr<emojis>> emojis_;

  stickerSet();

  stickerSet(std::int64_t id_, std::string const &title_, std::string const &name_, object_ptr<photoSize> &&thumbnail_, bool is_installed_, bool is_archived_, bool is_official_, bool is_animated_, bool is_masks_, bool is_viewed_, std::vector<object_ptr<sticker>> &&stickers_, std::vector<object_ptr<emojis>> &&emojis_);

  static const std::int32_t ID = 734588298;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class stickerSetInfo final : public Object {
 public:
  std::int64_t id_;
  std::string title_;
  std::string name_;
  object_ptr<photoSize> thumbnail_;
  bool is_installed_;
  bool is_archived_;
  bool is_official_;
  bool is_animated_;
  bool is_masks_;
  bool is_viewed_;
  std::int32_t size_;
  std::vector<object_ptr<sticker>> covers_;

  stickerSetInfo();

  stickerSetInfo(std::int64_t id_, std::string const &title_, std::string const &name_, object_ptr<photoSize> &&thumbnail_, bool is_installed_, bool is_archived_, bool is_official_, bool is_animated_, bool is_masks_, bool is_viewed_, std::int32_t size_, std::vector<object_ptr<sticker>> &&covers_);

  static const std::int32_t ID = 228054782;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class stickerSets final : public Object {
 public:
  std::int32_t total_count_;
  std::vector<object_ptr<stickerSetInfo>> sets_;

  stickerSets();

  stickerSets(std::int32_t total_count_, std::vector<object_ptr<stickerSetInfo>> &&sets_);

  static const std::int32_t ID = -1883828812;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class stickers final : public Object {
 public:
  std::vector<object_ptr<sticker>> stickers_;

  stickers();

  explicit stickers(std::vector<object_ptr<sticker>> &&stickers_);

  static const std::int32_t ID = 1974859260;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class storageStatistics final : public Object {
 public:
  std::int64_t size_;
  std::int32_t count_;
  std::vector<object_ptr<storageStatisticsByChat>> by_chat_;

  storageStatistics();

  storageStatistics(std::int64_t size_, std::int32_t count_, std::vector<object_ptr<storageStatisticsByChat>> &&by_chat_);

  static const std::int32_t ID = 217237013;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class storageStatisticsByChat final : public Object {
 public:
  std::int64_t chat_id_;
  std::int64_t size_;
  std::int32_t count_;
  std::vector<object_ptr<storageStatisticsByFileType>> by_file_type_;

  storageStatisticsByChat();

  storageStatisticsByChat(std::int64_t chat_id_, std::int64_t size_, std::int32_t count_, std::vector<object_ptr<storageStatisticsByFileType>> &&by_file_type_);

  static const std::int32_t ID = 635434531;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class storageStatisticsByFileType final : public Object {
 public:
  object_ptr<FileType> file_type_;
  std::int64_t size_;
  std::int32_t count_;

  storageStatisticsByFileType();

  storageStatisticsByFileType(object_ptr<FileType> &&file_type_, std::int64_t size_, std::int32_t count_);

  static const std::int32_t ID = 714012840;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class storageStatisticsFast final : public Object {
 public:
  std::int64_t files_size_;
  std::int32_t file_count_;
  std::int64_t database_size_;
  std::int64_t language_pack_database_size_;
  std::int64_t log_size_;

  storageStatisticsFast();

  storageStatisticsFast(std::int64_t files_size_, std::int32_t file_count_, std::int64_t database_size_, std::int64_t language_pack_database_size_, std::int64_t log_size_);

  static const std::int32_t ID = -884922271;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroup final : public Object {
 public:
  std::int32_t id_;
  std::string username_;
  std::int32_t date_;
  object_ptr<ChatMemberStatus> status_;
  std::int32_t member_count_;
  bool has_linked_chat_;
  bool has_location_;
  bool sign_messages_;
  bool is_slow_mode_enabled_;
  bool is_channel_;
  bool is_verified_;
  std::string restriction_reason_;
  bool is_scam_;

  supergroup();

  supergroup(std::int32_t id_, std::string const &username_, std::int32_t date_, object_ptr<ChatMemberStatus> &&status_, std::int32_t member_count_, bool has_linked_chat_, bool has_location_, bool sign_messages_, bool is_slow_mode_enabled_, bool is_channel_, bool is_verified_, std::string const &restriction_reason_, bool is_scam_);

  static const std::int32_t ID = -103091;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupFullInfo final : public Object {
 public:
  std::string description_;
  std::int32_t member_count_;
  std::int32_t administrator_count_;
  std::int32_t restricted_count_;
  std::int32_t banned_count_;
  std::int64_t linked_chat_id_;
  std::int32_t slow_mode_delay_;
  double slow_mode_delay_expires_in_;
  bool can_get_members_;
  bool can_set_username_;
  bool can_set_sticker_set_;
  bool can_set_location_;
  bool can_view_statistics_;
  bool is_all_history_available_;
  std::int64_t sticker_set_id_;
  object_ptr<chatLocation> location_;
  std::string invite_link_;
  std::int32_t upgraded_from_basic_group_id_;
  std::int64_t upgraded_from_max_message_id_;

  supergroupFullInfo();

  supergroupFullInfo(std::string const &description_, std::int32_t member_count_, std::int32_t administrator_count_, std::int32_t restricted_count_, std::int32_t banned_count_, std::int64_t linked_chat_id_, std::int32_t slow_mode_delay_, double slow_mode_delay_expires_in_, bool can_get_members_, bool can_set_username_, bool can_set_sticker_set_, bool can_set_location_, bool can_view_statistics_, bool is_all_history_available_, std::int64_t sticker_set_id_, object_ptr<chatLocation> &&location_, std::string const &invite_link_, std::int32_t upgraded_from_basic_group_id_, std::int64_t upgraded_from_max_message_id_);

  static const std::int32_t ID = -1562832718;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class SupergroupMembersFilter: public Object {
 public:
};

class supergroupMembersFilterRecent final : public SupergroupMembersFilter {
 public:

  supergroupMembersFilterRecent();

  static const std::int32_t ID = 1178199509;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterContacts final : public SupergroupMembersFilter {
 public:
  std::string query_;

  supergroupMembersFilterContacts();

  explicit supergroupMembersFilterContacts(std::string const &query_);

  static const std::int32_t ID = -1282910856;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterAdministrators final : public SupergroupMembersFilter {
 public:

  supergroupMembersFilterAdministrators();

  static const std::int32_t ID = -2097380265;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterSearch final : public SupergroupMembersFilter {
 public:
  std::string query_;

  supergroupMembersFilterSearch();

  explicit supergroupMembersFilterSearch(std::string const &query_);

  static const std::int32_t ID = -1696358469;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterRestricted final : public SupergroupMembersFilter {
 public:
  std::string query_;

  supergroupMembersFilterRestricted();

  explicit supergroupMembersFilterRestricted(std::string const &query_);

  static const std::int32_t ID = -1107800034;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterBanned final : public SupergroupMembersFilter {
 public:
  std::string query_;

  supergroupMembersFilterBanned();

  explicit supergroupMembersFilterBanned(std::string const &query_);

  static const std::int32_t ID = -1210621683;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class supergroupMembersFilterBots final : public SupergroupMembersFilter {
 public:

  supergroupMembersFilterBots();

  static const std::int32_t ID = 492138918;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tMeUrl final : public Object {
 public:
  std::string url_;
  object_ptr<TMeUrlType> type_;

  tMeUrl();

  tMeUrl(std::string const &url_, object_ptr<TMeUrlType> &&type_);

  static const std::int32_t ID = -1140786622;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class TMeUrlType: public Object {
 public:
};

class tMeUrlTypeUser final : public TMeUrlType {
 public:
  std::int32_t user_id_;

  tMeUrlTypeUser();

  explicit tMeUrlTypeUser(std::int32_t user_id_);

  static const std::int32_t ID = -1198700130;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tMeUrlTypeSupergroup final : public TMeUrlType {
 public:
  std::int64_t supergroup_id_;

  tMeUrlTypeSupergroup();

  explicit tMeUrlTypeSupergroup(std::int64_t supergroup_id_);

  static const std::int32_t ID = -1353369944;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tMeUrlTypeChatInvite final : public TMeUrlType {
 public:
  object_ptr<chatInviteLinkInfo> info_;

  tMeUrlTypeChatInvite();

  explicit tMeUrlTypeChatInvite(object_ptr<chatInviteLinkInfo> &&info_);

  static const std::int32_t ID = 313907785;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tMeUrlTypeStickerSet final : public TMeUrlType {
 public:
  std::int64_t sticker_set_id_;

  tMeUrlTypeStickerSet();

  explicit tMeUrlTypeStickerSet(std::int64_t sticker_set_id_);

  static const std::int32_t ID = 1602473196;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tMeUrls final : public Object {
 public:
  std::vector<object_ptr<tMeUrl>> urls_;

  tMeUrls();

  explicit tMeUrls(std::vector<object_ptr<tMeUrl>> &&urls_);

  static const std::int32_t ID = -1130595098;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class tdlibParameters final : public Object {
 public:
  bool use_test_dc_;
  std::string database_directory_;
  std::string files_directory_;
  bool use_file_database_;
  bool use_chat_info_database_;
  bool use_message_database_;
  bool use_secret_chats_;
  std::int32_t api_id_;
  std::string api_hash_;
  std::string system_language_code_;
  std::string device_model_;
  std::string system_version_;
  std::string application_version_;
  bool enable_storage_optimizer_;
  bool ignore_file_names_;

  tdlibParameters();

  tdlibParameters(bool use_test_dc_, std::string const &database_directory_, std::string const &files_directory_, bool use_file_database_, bool use_chat_info_database_, bool use_message_database_, bool use_secret_chats_, std::int32_t api_id_, std::string const &api_hash_, std::string const &system_language_code_, std::string const &device_model_, std::string const &system_version_, std::string const &application_version_, bool enable_storage_optimizer_, bool ignore_file_names_);

  static const std::int32_t ID = -761520773;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class temporaryPasswordState final : public Object {
 public:
  bool has_password_;
  std::int32_t valid_for_;

  temporaryPasswordState();

  temporaryPasswordState(bool has_password_, std::int32_t valid_for_);

  static const std::int32_t ID = 939837410;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class termsOfService final : public Object {
 public:
  object_ptr<formattedText> text_;
  std::int32_t min_user_age_;
  bool show_popup_;

  termsOfService();

  termsOfService(object_ptr<formattedText> &&text_, std::int32_t min_user_age_, bool show_popup_);

  static const std::int32_t ID = 739422597;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testBytes final : public Object {
 public:
  std::string value_;

  testBytes();

  explicit testBytes(std::string const &value_);

  static const std::int32_t ID = -1541225250;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testInt final : public Object {
 public:
  std::int32_t value_;

  testInt();

  explicit testInt(std::int32_t value_);

  static const std::int32_t ID = -574804983;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testString final : public Object {
 public:
  std::string value_;

  testString();

  explicit testString(std::string const &value_);

  static const std::int32_t ID = -27891572;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testVectorInt final : public Object {
 public:
  std::vector<std::int32_t> value_;

  testVectorInt();

  explicit testVectorInt(std::vector<std::int32_t> &&value_);

  static const std::int32_t ID = 593682027;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testVectorIntObject final : public Object {
 public:
  std::vector<object_ptr<testInt>> value_;

  testVectorIntObject();

  explicit testVectorIntObject(std::vector<object_ptr<testInt>> &&value_);

  static const std::int32_t ID = 125891546;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testVectorString final : public Object {
 public:
  std::vector<std::string> value_;

  testVectorString();

  explicit testVectorString(std::vector<std::string> &&value_);

  static const std::int32_t ID = 79339995;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testVectorStringObject final : public Object {
 public:
  std::vector<object_ptr<testString>> value_;

  testVectorStringObject();

  explicit testVectorStringObject(std::vector<object_ptr<testString>> &&value_);

  static const std::int32_t ID = 80780537;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class text final : public Object {
 public:
  std::string text_;

  text();

  explicit text(std::string const &text_);

  static const std::int32_t ID = 578181272;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntities final : public Object {
 public:
  std::vector<object_ptr<textEntity>> entities_;

  textEntities();

  explicit textEntities(std::vector<object_ptr<textEntity>> &&entities_);

  static const std::int32_t ID = -933199172;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntity final : public Object {
 public:
  std::int32_t offset_;
  std::int32_t length_;
  object_ptr<TextEntityType> type_;

  textEntity();

  textEntity(std::int32_t offset_, std::int32_t length_, object_ptr<TextEntityType> &&type_);

  static const std::int32_t ID = -1951688280;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class TextEntityType: public Object {
 public:
};

class textEntityTypeMention final : public TextEntityType {
 public:

  textEntityTypeMention();

  static const std::int32_t ID = 934535013;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeHashtag final : public TextEntityType {
 public:

  textEntityTypeHashtag();

  static const std::int32_t ID = -1023958307;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeCashtag final : public TextEntityType {
 public:

  textEntityTypeCashtag();

  static const std::int32_t ID = 1222915915;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeBotCommand final : public TextEntityType {
 public:

  textEntityTypeBotCommand();

  static const std::int32_t ID = -1150997581;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeUrl final : public TextEntityType {
 public:

  textEntityTypeUrl();

  static const std::int32_t ID = -1312762756;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeEmailAddress final : public TextEntityType {
 public:

  textEntityTypeEmailAddress();

  static const std::int32_t ID = 1425545249;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypePhoneNumber final : public TextEntityType {
 public:

  textEntityTypePhoneNumber();

  static const std::int32_t ID = -1160140246;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeBold final : public TextEntityType {
 public:

  textEntityTypeBold();

  static const std::int32_t ID = -1128210000;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeItalic final : public TextEntityType {
 public:

  textEntityTypeItalic();

  static const std::int32_t ID = -118253987;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeUnderline final : public TextEntityType {
 public:

  textEntityTypeUnderline();

  static const std::int32_t ID = 792317842;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeStrikethrough final : public TextEntityType {
 public:

  textEntityTypeStrikethrough();

  static const std::int32_t ID = 961529082;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeCode final : public TextEntityType {
 public:

  textEntityTypeCode();

  static const std::int32_t ID = -974534326;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypePre final : public TextEntityType {
 public:

  textEntityTypePre();

  static const std::int32_t ID = 1648958606;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypePreCode final : public TextEntityType {
 public:
  std::string language_;

  textEntityTypePreCode();

  explicit textEntityTypePreCode(std::string const &language_);

  static const std::int32_t ID = -945325397;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeTextUrl final : public TextEntityType {
 public:
  std::string url_;

  textEntityTypeTextUrl();

  explicit textEntityTypeTextUrl(std::string const &url_);

  static const std::int32_t ID = 445719651;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textEntityTypeMentionName final : public TextEntityType {
 public:
  std::int32_t user_id_;

  textEntityTypeMentionName();

  explicit textEntityTypeMentionName(std::int32_t user_id_);

  static const std::int32_t ID = -791517091;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class TextParseMode: public Object {
 public:
};

class textParseModeMarkdown final : public TextParseMode {
 public:
  std::int32_t version_;

  textParseModeMarkdown();

  explicit textParseModeMarkdown(std::int32_t version_);

  static const std::int32_t ID = 360073407;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class textParseModeHTML final : public TextParseMode {
 public:

  textParseModeHTML();

  static const std::int32_t ID = 1660208627;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class TopChatCategory: public Object {
 public:
};

class topChatCategoryUsers final : public TopChatCategory {
 public:

  topChatCategoryUsers();

  static const std::int32_t ID = 1026706816;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryBots final : public TopChatCategory {
 public:

  topChatCategoryBots();

  static const std::int32_t ID = -1577129195;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryGroups final : public TopChatCategory {
 public:

  topChatCategoryGroups();

  static const std::int32_t ID = 1530056846;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryChannels final : public TopChatCategory {
 public:

  topChatCategoryChannels();

  static const std::int32_t ID = -500825885;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryInlineBots final : public TopChatCategory {
 public:

  topChatCategoryInlineBots();

  static const std::int32_t ID = 377023356;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryCalls final : public TopChatCategory {
 public:

  topChatCategoryCalls();

  static const std::int32_t ID = 356208861;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class topChatCategoryForwardChats final : public TopChatCategory {
 public:

  topChatCategoryForwardChats();

  static const std::int32_t ID = 1695922133;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class Update: public Object {
 public:
};

class updateAuthorizationState final : public Update {
 public:
  object_ptr<AuthorizationState> authorization_state_;

  updateAuthorizationState();

  explicit updateAuthorizationState(object_ptr<AuthorizationState> &&authorization_state_);

  static const std::int32_t ID = 1622347490;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewMessage final : public Update {
 public:
  object_ptr<message> message_;

  updateNewMessage();

  explicit updateNewMessage(object_ptr<message> &&message_);

  static const std::int32_t ID = -563105266;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageSendAcknowledged final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  updateMessageSendAcknowledged();

  updateMessageSendAcknowledged(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = 1302843961;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageSendSucceeded final : public Update {
 public:
  object_ptr<message> message_;
  std::int64_t old_message_id_;

  updateMessageSendSucceeded();

  updateMessageSendSucceeded(object_ptr<message> &&message_, std::int64_t old_message_id_);

  static const std::int32_t ID = 1815715197;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageSendFailed final : public Update {
 public:
  object_ptr<message> message_;
  std::int64_t old_message_id_;
  std::int32_t error_code_;
  std::string error_message_;

  updateMessageSendFailed();

  updateMessageSendFailed(object_ptr<message> &&message_, std::int64_t old_message_id_, std::int32_t error_code_, std::string const &error_message_);

  static const std::int32_t ID = -1032335779;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageContent final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<MessageContent> new_content_;

  updateMessageContent();

  updateMessageContent(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<MessageContent> &&new_content_);

  static const std::int32_t ID = 506903332;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageEdited final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t edit_date_;
  object_ptr<ReplyMarkup> reply_markup_;

  updateMessageEdited();

  updateMessageEdited(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t edit_date_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = -559545626;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageViews final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t views_;

  updateMessageViews();

  updateMessageViews(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t views_);

  static const std::int32_t ID = -1854131125;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageContentOpened final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  updateMessageContentOpened();

  updateMessageContentOpened(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -1520523131;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageMentionRead final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t unread_mention_count_;

  updateMessageMentionRead();

  updateMessageMentionRead(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t unread_mention_count_);

  static const std::int32_t ID = -252228282;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateMessageLiveLocationViewed final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  updateMessageLiveLocationViewed();

  updateMessageLiveLocationViewed(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -1308260971;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewChat final : public Update {
 public:
  object_ptr<chat> chat_;

  updateNewChat();

  explicit updateNewChat(object_ptr<chat> &&chat_);

  static const std::int32_t ID = 2075757773;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatChatList final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatList> chat_list_;

  updateChatChatList();

  updateChatChatList(std::int64_t chat_id_, object_ptr<ChatList> &&chat_list_);

  static const std::int32_t ID = -170455894;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatTitle final : public Update {
 public:
  std::int64_t chat_id_;
  std::string title_;

  updateChatTitle();

  updateChatTitle(std::int64_t chat_id_, std::string const &title_);

  static const std::int32_t ID = -175405660;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatPhoto final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<chatPhoto> photo_;

  updateChatPhoto();

  updateChatPhoto(std::int64_t chat_id_, object_ptr<chatPhoto> &&photo_);

  static const std::int32_t ID = -209353966;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatPermissions final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<chatPermissions> permissions_;

  updateChatPermissions();

  updateChatPermissions(std::int64_t chat_id_, object_ptr<chatPermissions> &&permissions_);

  static const std::int32_t ID = -1622010003;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatLastMessage final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<message> last_message_;
  std::int64_t order_;

  updateChatLastMessage();

  updateChatLastMessage(std::int64_t chat_id_, object_ptr<message> &&last_message_, std::int64_t order_);

  static const std::int32_t ID = 580348828;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatOrder final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t order_;

  updateChatOrder();

  updateChatOrder(std::int64_t chat_id_, std::int64_t order_);

  static const std::int32_t ID = -1601888026;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatIsPinned final : public Update {
 public:
  std::int64_t chat_id_;
  bool is_pinned_;
  std::int64_t order_;

  updateChatIsPinned();

  updateChatIsPinned(std::int64_t chat_id_, bool is_pinned_, std::int64_t order_);

  static const std::int32_t ID = 488876260;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatIsMarkedAsUnread final : public Update {
 public:
  std::int64_t chat_id_;
  bool is_marked_as_unread_;

  updateChatIsMarkedAsUnread();

  updateChatIsMarkedAsUnread(std::int64_t chat_id_, bool is_marked_as_unread_);

  static const std::int32_t ID = 1468347188;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatIsSponsored final : public Update {
 public:
  std::int64_t chat_id_;
  bool is_sponsored_;
  std::int64_t order_;

  updateChatIsSponsored();

  updateChatIsSponsored(std::int64_t chat_id_, bool is_sponsored_, std::int64_t order_);

  static const std::int32_t ID = -1196180070;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatHasScheduledMessages final : public Update {
 public:
  std::int64_t chat_id_;
  bool has_scheduled_messages_;

  updateChatHasScheduledMessages();

  updateChatHasScheduledMessages(std::int64_t chat_id_, bool has_scheduled_messages_);

  static const std::int32_t ID = 2064958167;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatDefaultDisableNotification final : public Update {
 public:
  std::int64_t chat_id_;
  bool default_disable_notification_;

  updateChatDefaultDisableNotification();

  updateChatDefaultDisableNotification(std::int64_t chat_id_, bool default_disable_notification_);

  static const std::int32_t ID = 464087707;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatReadInbox final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t last_read_inbox_message_id_;
  std::int32_t unread_count_;

  updateChatReadInbox();

  updateChatReadInbox(std::int64_t chat_id_, std::int64_t last_read_inbox_message_id_, std::int32_t unread_count_);

  static const std::int32_t ID = -797952281;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatReadOutbox final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t last_read_outbox_message_id_;

  updateChatReadOutbox();

  updateChatReadOutbox(std::int64_t chat_id_, std::int64_t last_read_outbox_message_id_);

  static const std::int32_t ID = 708334213;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatUnreadMentionCount final : public Update {
 public:
  std::int64_t chat_id_;
  std::int32_t unread_mention_count_;

  updateChatUnreadMentionCount();

  updateChatUnreadMentionCount(std::int64_t chat_id_, std::int32_t unread_mention_count_);

  static const std::int32_t ID = -2131461348;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatNotificationSettings final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<chatNotificationSettings> notification_settings_;

  updateChatNotificationSettings();

  updateChatNotificationSettings(std::int64_t chat_id_, object_ptr<chatNotificationSettings> &&notification_settings_);

  static const std::int32_t ID = -803163050;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateScopeNotificationSettings final : public Update {
 public:
  object_ptr<NotificationSettingsScope> scope_;
  object_ptr<scopeNotificationSettings> notification_settings_;

  updateScopeNotificationSettings();

  updateScopeNotificationSettings(object_ptr<NotificationSettingsScope> &&scope_, object_ptr<scopeNotificationSettings> &&notification_settings_);

  static const std::int32_t ID = -1203975309;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatActionBar final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatActionBar> action_bar_;

  updateChatActionBar();

  updateChatActionBar(std::int64_t chat_id_, object_ptr<ChatActionBar> &&action_bar_);

  static const std::int32_t ID = -643671870;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatPinnedMessage final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t pinned_message_id_;

  updateChatPinnedMessage();

  updateChatPinnedMessage(std::int64_t chat_id_, std::int64_t pinned_message_id_);

  static const std::int32_t ID = 802160507;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatReplyMarkup final : public Update {
 public:
  std::int64_t chat_id_;
  std::int64_t reply_markup_message_id_;

  updateChatReplyMarkup();

  updateChatReplyMarkup(std::int64_t chat_id_, std::int64_t reply_markup_message_id_);

  static const std::int32_t ID = 1309386144;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatDraftMessage final : public Update {
 public:
  std::int64_t chat_id_;
  object_ptr<draftMessage> draft_message_;
  std::int64_t order_;

  updateChatDraftMessage();

  updateChatDraftMessage(std::int64_t chat_id_, object_ptr<draftMessage> &&draft_message_, std::int64_t order_);

  static const std::int32_t ID = -1436617498;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateChatOnlineMemberCount final : public Update {
 public:
  std::int64_t chat_id_;
  std::int32_t online_member_count_;

  updateChatOnlineMemberCount();

  updateChatOnlineMemberCount(std::int64_t chat_id_, std::int32_t online_member_count_);

  static const std::int32_t ID = 487369373;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNotification final : public Update {
 public:
  std::int32_t notification_group_id_;
  object_ptr<notification> notification_;

  updateNotification();

  updateNotification(std::int32_t notification_group_id_, object_ptr<notification> &&notification_);

  static const std::int32_t ID = -1897496876;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNotificationGroup final : public Update {
 public:
  std::int32_t notification_group_id_;
  object_ptr<NotificationGroupType> type_;
  std::int64_t chat_id_;
  std::int64_t notification_settings_chat_id_;
  bool is_silent_;
  std::int32_t total_count_;
  std::vector<object_ptr<notification>> added_notifications_;
  std::vector<std::int32_t> removed_notification_ids_;

  updateNotificationGroup();

  updateNotificationGroup(std::int32_t notification_group_id_, object_ptr<NotificationGroupType> &&type_, std::int64_t chat_id_, std::int64_t notification_settings_chat_id_, bool is_silent_, std::int32_t total_count_, std::vector<object_ptr<notification>> &&added_notifications_, std::vector<std::int32_t> &&removed_notification_ids_);

  static const std::int32_t ID = -2049005665;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateActiveNotifications final : public Update {
 public:
  std::vector<object_ptr<notificationGroup>> groups_;

  updateActiveNotifications();

  explicit updateActiveNotifications(std::vector<object_ptr<notificationGroup>> &&groups_);

  static const std::int32_t ID = -1306672221;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateHavePendingNotifications final : public Update {
 public:
  bool have_delayed_notifications_;
  bool have_unreceived_notifications_;

  updateHavePendingNotifications();

  updateHavePendingNotifications(bool have_delayed_notifications_, bool have_unreceived_notifications_);

  static const std::int32_t ID = 179233243;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateDeleteMessages final : public Update {
 public:
  std::int64_t chat_id_;
  std::vector<std::int64_t> message_ids_;
  bool is_permanent_;
  bool from_cache_;

  updateDeleteMessages();

  updateDeleteMessages(std::int64_t chat_id_, std::vector<std::int64_t> &&message_ids_, bool is_permanent_, bool from_cache_);

  static const std::int32_t ID = 1669252686;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUserChatAction final : public Update {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;
  object_ptr<ChatAction> action_;

  updateUserChatAction();

  updateUserChatAction(std::int64_t chat_id_, std::int32_t user_id_, object_ptr<ChatAction> &&action_);

  static const std::int32_t ID = 1444133514;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUserStatus final : public Update {
 public:
  std::int32_t user_id_;
  object_ptr<UserStatus> status_;

  updateUserStatus();

  updateUserStatus(std::int32_t user_id_, object_ptr<UserStatus> &&status_);

  static const std::int32_t ID = -1443545195;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUser final : public Update {
 public:
  object_ptr<user> user_;

  updateUser();

  explicit updateUser(object_ptr<user> &&user_);

  static const std::int32_t ID = 1183394041;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateBasicGroup final : public Update {
 public:
  object_ptr<basicGroup> basic_group_;

  updateBasicGroup();

  explicit updateBasicGroup(object_ptr<basicGroup> &&basic_group_);

  static const std::int32_t ID = -1003239581;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateSupergroup final : public Update {
 public:
  object_ptr<supergroup> supergroup_;

  updateSupergroup();

  explicit updateSupergroup(object_ptr<supergroup> &&supergroup_);

  static const std::int32_t ID = -76782300;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateSecretChat final : public Update {
 public:
  object_ptr<secretChat> secret_chat_;

  updateSecretChat();

  explicit updateSecretChat(object_ptr<secretChat> &&secret_chat_);

  static const std::int32_t ID = -1666903253;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUserFullInfo final : public Update {
 public:
  std::int32_t user_id_;
  object_ptr<userFullInfo> user_full_info_;

  updateUserFullInfo();

  updateUserFullInfo(std::int32_t user_id_, object_ptr<userFullInfo> &&user_full_info_);

  static const std::int32_t ID = 222103874;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateBasicGroupFullInfo final : public Update {
 public:
  std::int32_t basic_group_id_;
  object_ptr<basicGroupFullInfo> basic_group_full_info_;

  updateBasicGroupFullInfo();

  updateBasicGroupFullInfo(std::int32_t basic_group_id_, object_ptr<basicGroupFullInfo> &&basic_group_full_info_);

  static const std::int32_t ID = 924030531;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateSupergroupFullInfo final : public Update {
 public:
  std::int32_t supergroup_id_;
  object_ptr<supergroupFullInfo> supergroup_full_info_;

  updateSupergroupFullInfo();

  updateSupergroupFullInfo(std::int32_t supergroup_id_, object_ptr<supergroupFullInfo> &&supergroup_full_info_);

  static const std::int32_t ID = 1288828758;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateServiceNotification final : public Update {
 public:
  std::string type_;
  object_ptr<MessageContent> content_;

  updateServiceNotification();

  updateServiceNotification(std::string const &type_, object_ptr<MessageContent> &&content_);

  static const std::int32_t ID = 1318622637;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateFile final : public Update {
 public:
  object_ptr<file> file_;

  updateFile();

  explicit updateFile(object_ptr<file> &&file_);

  static const std::int32_t ID = 114132831;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateFileGenerationStart final : public Update {
 public:
  std::int64_t generation_id_;
  std::string original_path_;
  std::string destination_path_;
  std::string conversion_;

  updateFileGenerationStart();

  updateFileGenerationStart(std::int64_t generation_id_, std::string const &original_path_, std::string const &destination_path_, std::string const &conversion_);

  static const std::int32_t ID = 216817388;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateFileGenerationStop final : public Update {
 public:
  std::int64_t generation_id_;

  updateFileGenerationStop();

  explicit updateFileGenerationStop(std::int64_t generation_id_);

  static const std::int32_t ID = -1894449685;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateCall final : public Update {
 public:
  object_ptr<call> call_;

  updateCall();

  explicit updateCall(object_ptr<call> &&call_);

  static const std::int32_t ID = 1337184477;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUserPrivacySettingRules final : public Update {
 public:
  object_ptr<UserPrivacySetting> setting_;
  object_ptr<userPrivacySettingRules> rules_;

  updateUserPrivacySettingRules();

  updateUserPrivacySettingRules(object_ptr<UserPrivacySetting> &&setting_, object_ptr<userPrivacySettingRules> &&rules_);

  static const std::int32_t ID = -912960778;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUnreadMessageCount final : public Update {
 public:
  object_ptr<ChatList> chat_list_;
  std::int32_t unread_count_;
  std::int32_t unread_unmuted_count_;

  updateUnreadMessageCount();

  updateUnreadMessageCount(object_ptr<ChatList> &&chat_list_, std::int32_t unread_count_, std::int32_t unread_unmuted_count_);

  static const std::int32_t ID = 78987721;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUnreadChatCount final : public Update {
 public:
  object_ptr<ChatList> chat_list_;
  std::int32_t total_count_;
  std::int32_t unread_count_;
  std::int32_t unread_unmuted_count_;
  std::int32_t marked_as_unread_count_;
  std::int32_t marked_as_unread_unmuted_count_;

  updateUnreadChatCount();

  updateUnreadChatCount(object_ptr<ChatList> &&chat_list_, std::int32_t total_count_, std::int32_t unread_count_, std::int32_t unread_unmuted_count_, std::int32_t marked_as_unread_count_, std::int32_t marked_as_unread_unmuted_count_);

  static const std::int32_t ID = 1994494530;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateOption final : public Update {
 public:
  std::string name_;
  object_ptr<OptionValue> value_;

  updateOption();

  updateOption(std::string const &name_, object_ptr<OptionValue> &&value_);

  static const std::int32_t ID = 900822020;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateInstalledStickerSets final : public Update {
 public:
  bool is_masks_;
  std::vector<std::int64_t> sticker_set_ids_;

  updateInstalledStickerSets();

  updateInstalledStickerSets(bool is_masks_, std::vector<std::int64_t> &&sticker_set_ids_);

  static const std::int32_t ID = 1125575977;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateTrendingStickerSets final : public Update {
 public:
  object_ptr<stickerSets> sticker_sets_;

  updateTrendingStickerSets();

  explicit updateTrendingStickerSets(object_ptr<stickerSets> &&sticker_sets_);

  static const std::int32_t ID = 450714593;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateRecentStickers final : public Update {
 public:
  bool is_attached_;
  std::vector<std::int32_t> sticker_ids_;

  updateRecentStickers();

  updateRecentStickers(bool is_attached_, std::vector<std::int32_t> &&sticker_ids_);

  static const std::int32_t ID = 1906403540;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateFavoriteStickers final : public Update {
 public:
  std::vector<std::int32_t> sticker_ids_;

  updateFavoriteStickers();

  explicit updateFavoriteStickers(std::vector<std::int32_t> &&sticker_ids_);

  static const std::int32_t ID = 1662240999;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateSavedAnimations final : public Update {
 public:
  std::vector<std::int32_t> animation_ids_;

  updateSavedAnimations();

  explicit updateSavedAnimations(std::vector<std::int32_t> &&animation_ids_);

  static const std::int32_t ID = 65563814;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateSelectedBackground final : public Update {
 public:
  bool for_dark_theme_;
  object_ptr<background> background_;

  updateSelectedBackground();

  updateSelectedBackground(bool for_dark_theme_, object_ptr<background> &&background_);

  static const std::int32_t ID = -1715658659;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateLanguagePackStrings final : public Update {
 public:
  std::string localization_target_;
  std::string language_pack_id_;
  std::vector<object_ptr<languagePackString>> strings_;

  updateLanguagePackStrings();

  updateLanguagePackStrings(std::string const &localization_target_, std::string const &language_pack_id_, std::vector<object_ptr<languagePackString>> &&strings_);

  static const std::int32_t ID = -1056319886;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateConnectionState final : public Update {
 public:
  object_ptr<ConnectionState> state_;

  updateConnectionState();

  explicit updateConnectionState(object_ptr<ConnectionState> &&state_);

  static const std::int32_t ID = 1469292078;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateTermsOfService final : public Update {
 public:
  std::string terms_of_service_id_;
  object_ptr<termsOfService> terms_of_service_;

  updateTermsOfService();

  updateTermsOfService(std::string const &terms_of_service_id_, object_ptr<termsOfService> &&terms_of_service_);

  static const std::int32_t ID = -1304640162;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateUsersNearby final : public Update {
 public:
  std::vector<object_ptr<chatNearby>> users_nearby_;

  updateUsersNearby();

  explicit updateUsersNearby(std::vector<object_ptr<chatNearby>> &&users_nearby_);

  static const std::int32_t ID = -1517109163;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewInlineQuery final : public Update {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  object_ptr<location> user_location_;
  std::string query_;
  std::string offset_;

  updateNewInlineQuery();

  updateNewInlineQuery(std::int64_t id_, std::int32_t sender_user_id_, object_ptr<location> &&user_location_, std::string const &query_, std::string const &offset_);

  static const std::int32_t ID = 2064730634;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewChosenInlineResult final : public Update {
 public:
  std::int32_t sender_user_id_;
  object_ptr<location> user_location_;
  std::string query_;
  std::string result_id_;
  std::string inline_message_id_;

  updateNewChosenInlineResult();

  updateNewChosenInlineResult(std::int32_t sender_user_id_, object_ptr<location> &&user_location_, std::string const &query_, std::string const &result_id_, std::string const &inline_message_id_);

  static const std::int32_t ID = 527526965;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewCallbackQuery final : public Update {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int64_t chat_instance_;
  object_ptr<CallbackQueryPayload> payload_;

  updateNewCallbackQuery();

  updateNewCallbackQuery(std::int64_t id_, std::int32_t sender_user_id_, std::int64_t chat_id_, std::int64_t message_id_, std::int64_t chat_instance_, object_ptr<CallbackQueryPayload> &&payload_);

  static const std::int32_t ID = -2044226370;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewInlineCallbackQuery final : public Update {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  std::string inline_message_id_;
  std::int64_t chat_instance_;
  object_ptr<CallbackQueryPayload> payload_;

  updateNewInlineCallbackQuery();

  updateNewInlineCallbackQuery(std::int64_t id_, std::int32_t sender_user_id_, std::string const &inline_message_id_, std::int64_t chat_instance_, object_ptr<CallbackQueryPayload> &&payload_);

  static const std::int32_t ID = -1879154829;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewShippingQuery final : public Update {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  std::string invoice_payload_;
  object_ptr<address> shipping_address_;

  updateNewShippingQuery();

  updateNewShippingQuery(std::int64_t id_, std::int32_t sender_user_id_, std::string const &invoice_payload_, object_ptr<address> &&shipping_address_);

  static const std::int32_t ID = -817474682;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewPreCheckoutQuery final : public Update {
 public:
  std::int64_t id_;
  std::int32_t sender_user_id_;
  std::string currency_;
  std::int64_t total_amount_;
  std::string invoice_payload_;
  std::string shipping_option_id_;
  object_ptr<orderInfo> order_info_;

  updateNewPreCheckoutQuery();

  updateNewPreCheckoutQuery(std::int64_t id_, std::int32_t sender_user_id_, std::string const &currency_, std::int64_t total_amount_, std::string const &invoice_payload_, std::string const &shipping_option_id_, object_ptr<orderInfo> &&order_info_);

  static const std::int32_t ID = 87964006;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewCustomEvent final : public Update {
 public:
  std::string event_;

  updateNewCustomEvent();

  explicit updateNewCustomEvent(std::string const &event_);

  static const std::int32_t ID = 1994222092;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updateNewCustomQuery final : public Update {
 public:
  std::int64_t id_;
  std::string data_;
  std::int32_t timeout_;

  updateNewCustomQuery();

  updateNewCustomQuery(std::int64_t id_, std::string const &data_, std::int32_t timeout_);

  static const std::int32_t ID = -687670874;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updatePoll final : public Update {
 public:
  object_ptr<poll> poll_;

  updatePoll();

  explicit updatePoll(object_ptr<poll> &&poll_);

  static const std::int32_t ID = -1771342902;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updatePollAnswer final : public Update {
 public:
  std::int64_t poll_id_;
  std::int32_t user_id_;
  std::vector<std::int32_t> option_ids_;

  updatePollAnswer();

  updatePollAnswer(std::int64_t poll_id_, std::int32_t user_id_, std::vector<std::int32_t> &&option_ids_);

  static const std::int32_t ID = 1606139344;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class updates final : public Object {
 public:
  std::vector<object_ptr<Update>> updates_;

  updates();

  explicit updates(std::vector<object_ptr<Update>> &&updates_);

  static const std::int32_t ID = 475842347;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class user final : public Object {
 public:
  std::int32_t id_;
  std::string first_name_;
  std::string last_name_;
  std::string username_;
  std::string phone_number_;
  object_ptr<UserStatus> status_;
  object_ptr<profilePhoto> profile_photo_;
  bool is_contact_;
  bool is_mutual_contact_;
  bool is_verified_;
  bool is_support_;
  std::string restriction_reason_;
  bool is_scam_;
  bool have_access_;
  object_ptr<UserType> type_;
  std::string language_code_;

  user();

  user(std::int32_t id_, std::string const &first_name_, std::string const &last_name_, std::string const &username_, std::string const &phone_number_, object_ptr<UserStatus> &&status_, object_ptr<profilePhoto> &&profile_photo_, bool is_contact_, bool is_mutual_contact_, bool is_verified_, bool is_support_, std::string const &restriction_reason_, bool is_scam_, bool have_access_, object_ptr<UserType> &&type_, std::string const &language_code_);

  static const std::int32_t ID = -824771497;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userFullInfo final : public Object {
 public:
  bool is_blocked_;
  bool can_be_called_;
  bool has_private_calls_;
  bool need_phone_number_privacy_exception_;
  std::string bio_;
  std::string share_text_;
  std::int32_t group_in_common_count_;
  object_ptr<botInfo> bot_info_;

  userFullInfo();

  userFullInfo(bool is_blocked_, bool can_be_called_, bool has_private_calls_, bool need_phone_number_privacy_exception_, std::string const &bio_, std::string const &share_text_, std::int32_t group_in_common_count_, object_ptr<botInfo> &&bot_info_);

  static const std::int32_t ID = 333888500;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class UserPrivacySetting: public Object {
 public:
};

class userPrivacySettingShowStatus final : public UserPrivacySetting {
 public:

  userPrivacySettingShowStatus();

  static const std::int32_t ID = 1862829310;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingShowProfilePhoto final : public UserPrivacySetting {
 public:

  userPrivacySettingShowProfilePhoto();

  static const std::int32_t ID = 1408485877;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingShowLinkInForwardedMessages final : public UserPrivacySetting {
 public:

  userPrivacySettingShowLinkInForwardedMessages();

  static const std::int32_t ID = 592688870;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingShowPhoneNumber final : public UserPrivacySetting {
 public:

  userPrivacySettingShowPhoneNumber();

  static const std::int32_t ID = -791567831;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingAllowChatInvites final : public UserPrivacySetting {
 public:

  userPrivacySettingAllowChatInvites();

  static const std::int32_t ID = 1271668007;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingAllowCalls final : public UserPrivacySetting {
 public:

  userPrivacySettingAllowCalls();

  static const std::int32_t ID = -906967291;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingAllowPeerToPeerCalls final : public UserPrivacySetting {
 public:

  userPrivacySettingAllowPeerToPeerCalls();

  static const std::int32_t ID = 352500032;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingAllowFindingByPhoneNumber final : public UserPrivacySetting {
 public:

  userPrivacySettingAllowFindingByPhoneNumber();

  static const std::int32_t ID = -1846645423;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class UserPrivacySettingRule: public Object {
 public:
};

class userPrivacySettingRuleAllowAll final : public UserPrivacySettingRule {
 public:

  userPrivacySettingRuleAllowAll();

  static const std::int32_t ID = -1967186881;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleAllowContacts final : public UserPrivacySettingRule {
 public:

  userPrivacySettingRuleAllowContacts();

  static const std::int32_t ID = -1892733680;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleAllowUsers final : public UserPrivacySettingRule {
 public:
  std::vector<std::int32_t> user_ids_;

  userPrivacySettingRuleAllowUsers();

  explicit userPrivacySettingRuleAllowUsers(std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = 427601278;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleAllowChatMembers final : public UserPrivacySettingRule {
 public:
  std::vector<std::int64_t> chat_ids_;

  userPrivacySettingRuleAllowChatMembers();

  explicit userPrivacySettingRuleAllowChatMembers(std::vector<std::int64_t> &&chat_ids_);

  static const std::int32_t ID = -2048749863;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleRestrictAll final : public UserPrivacySettingRule {
 public:

  userPrivacySettingRuleRestrictAll();

  static const std::int32_t ID = -1406495408;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleRestrictContacts final : public UserPrivacySettingRule {
 public:

  userPrivacySettingRuleRestrictContacts();

  static const std::int32_t ID = 1008389378;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleRestrictUsers final : public UserPrivacySettingRule {
 public:
  std::vector<std::int32_t> user_ids_;

  userPrivacySettingRuleRestrictUsers();

  explicit userPrivacySettingRuleRestrictUsers(std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = 2119951802;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRuleRestrictChatMembers final : public UserPrivacySettingRule {
 public:
  std::vector<std::int64_t> chat_ids_;

  userPrivacySettingRuleRestrictChatMembers();

  explicit userPrivacySettingRuleRestrictChatMembers(std::vector<std::int64_t> &&chat_ids_);

  static const std::int32_t ID = 392530897;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userPrivacySettingRules final : public Object {
 public:
  std::vector<object_ptr<UserPrivacySettingRule>> rules_;

  userPrivacySettingRules();

  explicit userPrivacySettingRules(std::vector<object_ptr<UserPrivacySettingRule>> &&rules_);

  static const std::int32_t ID = 322477541;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userProfilePhoto final : public Object {
 public:
  std::int64_t id_;
  std::int32_t added_date_;
  std::vector<object_ptr<photoSize>> sizes_;

  userProfilePhoto();

  userProfilePhoto(std::int64_t id_, std::int32_t added_date_, std::vector<object_ptr<photoSize>> &&sizes_);

  static const std::int32_t ID = -1882596466;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userProfilePhotos final : public Object {
 public:
  std::int32_t total_count_;
  std::vector<object_ptr<userProfilePhoto>> photos_;

  userProfilePhotos();

  userProfilePhotos(std::int32_t total_count_, std::vector<object_ptr<userProfilePhoto>> &&photos_);

  static const std::int32_t ID = 1512709690;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class UserStatus: public Object {
 public:
};

class userStatusEmpty final : public UserStatus {
 public:

  userStatusEmpty();

  static const std::int32_t ID = 164646985;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userStatusOnline final : public UserStatus {
 public:
  std::int32_t expires_;

  userStatusOnline();

  explicit userStatusOnline(std::int32_t expires_);

  static const std::int32_t ID = -1529460876;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userStatusOffline final : public UserStatus {
 public:
  std::int32_t was_online_;

  userStatusOffline();

  explicit userStatusOffline(std::int32_t was_online_);

  static const std::int32_t ID = -759984891;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userStatusRecently final : public UserStatus {
 public:

  userStatusRecently();

  static const std::int32_t ID = -496024847;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userStatusLastWeek final : public UserStatus {
 public:

  userStatusLastWeek();

  static const std::int32_t ID = 129960444;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userStatusLastMonth final : public UserStatus {
 public:

  userStatusLastMonth();

  static const std::int32_t ID = 2011940674;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class UserType: public Object {
 public:
};

class userTypeRegular final : public UserType {
 public:

  userTypeRegular();

  static const std::int32_t ID = -598644325;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userTypeDeleted final : public UserType {
 public:

  userTypeDeleted();

  static const std::int32_t ID = -1807729372;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userTypeBot final : public UserType {
 public:
  bool can_join_groups_;
  bool can_read_all_group_messages_;
  bool is_inline_;
  std::string inline_query_placeholder_;
  bool need_location_;

  userTypeBot();

  userTypeBot(bool can_join_groups_, bool can_read_all_group_messages_, bool is_inline_, std::string const &inline_query_placeholder_, bool need_location_);

  static const std::int32_t ID = 1262387765;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class userTypeUnknown final : public UserType {
 public:

  userTypeUnknown();

  static const std::int32_t ID = -724541123;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class users final : public Object {
 public:
  std::int32_t total_count_;
  std::vector<std::int32_t> user_ids_;

  users();

  users(std::int32_t total_count_, std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = 273760088;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class validatedOrderInfo final : public Object {
 public:
  std::string order_info_id_;
  std::vector<object_ptr<shippingOption>> shipping_options_;

  validatedOrderInfo();

  validatedOrderInfo(std::string const &order_info_id_, std::vector<object_ptr<shippingOption>> &&shipping_options_);

  static const std::int32_t ID = 1511451484;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class venue final : public Object {
 public:
  object_ptr<location> location_;
  std::string title_;
  std::string address_;
  std::string provider_;
  std::string id_;
  std::string type_;

  venue();

  venue(object_ptr<location> &&location_, std::string const &title_, std::string const &address_, std::string const &provider_, std::string const &id_, std::string const &type_);

  static const std::int32_t ID = 1070406393;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class video final : public Object {
 public:
  std::int32_t duration_;
  std::int32_t width_;
  std::int32_t height_;
  std::string file_name_;
  std::string mime_type_;
  bool has_stickers_;
  bool supports_streaming_;
  object_ptr<minithumbnail> minithumbnail_;
  object_ptr<photoSize> thumbnail_;
  object_ptr<file> video_;

  video();

  video(std::int32_t duration_, std::int32_t width_, std::int32_t height_, std::string const &file_name_, std::string const &mime_type_, bool has_stickers_, bool supports_streaming_, object_ptr<minithumbnail> &&minithumbnail_, object_ptr<photoSize> &&thumbnail_, object_ptr<file> &&video_);

  static const std::int32_t ID = -536898740;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class videoNote final : public Object {
 public:
  std::int32_t duration_;
  std::int32_t length_;
  object_ptr<minithumbnail> minithumbnail_;
  object_ptr<photoSize> thumbnail_;
  object_ptr<file> video_;

  videoNote();

  videoNote(std::int32_t duration_, std::int32_t length_, object_ptr<minithumbnail> &&minithumbnail_, object_ptr<photoSize> &&thumbnail_, object_ptr<file> &&video_);

  static const std::int32_t ID = -1080075672;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class voiceNote final : public Object {
 public:
  std::int32_t duration_;
  std::string waveform_;
  std::string mime_type_;
  object_ptr<file> voice_;

  voiceNote();

  voiceNote(std::int32_t duration_, std::string const &waveform_, std::string const &mime_type_, object_ptr<file> &&voice_);

  static const std::int32_t ID = -2066012058;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class webPage final : public Object {
 public:
  std::string url_;
  std::string display_url_;
  std::string type_;
  std::string site_name_;
  std::string title_;
  std::string description_;
  object_ptr<photo> photo_;
  std::string embed_url_;
  std::string embed_type_;
  std::int32_t embed_width_;
  std::int32_t embed_height_;
  std::int32_t duration_;
  std::string author_;
  object_ptr<animation> animation_;
  object_ptr<audio> audio_;
  object_ptr<document> document_;
  object_ptr<sticker> sticker_;
  object_ptr<video> video_;
  object_ptr<videoNote> video_note_;
  object_ptr<voiceNote> voice_note_;
  std::int32_t instant_view_version_;

  webPage();

  webPage(std::string const &url_, std::string const &display_url_, std::string const &type_, std::string const &site_name_, std::string const &title_, std::string const &description_, object_ptr<photo> &&photo_, std::string const &embed_url_, std::string const &embed_type_, std::int32_t embed_width_, std::int32_t embed_height_, std::int32_t duration_, std::string const &author_, object_ptr<animation> &&animation_, object_ptr<audio> &&audio_, object_ptr<document> &&document_, object_ptr<sticker> &&sticker_, object_ptr<video> &&video_, object_ptr<videoNote> &&video_note_, object_ptr<voiceNote> &&voice_note_, std::int32_t instant_view_version_);

  static const std::int32_t ID = 1092898169;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class webPageInstantView final : public Object {
 public:
  std::vector<object_ptr<PageBlock>> page_blocks_;
  std::int32_t version_;
  std::string url_;
  bool is_rtl_;
  bool is_full_;

  webPageInstantView();

  webPageInstantView(std::vector<object_ptr<PageBlock>> &&page_blocks_, std::int32_t version_, std::string const &url_, bool is_rtl_, bool is_full_);

  static const std::int32_t ID = 957287214;
  std::int32_t get_id() const final {
    return ID;
  }

  void store(TlStorerToString &s, const char *field_name) const final;
};

class acceptCall final : public Function {
 public:
  std::int32_t call_id_;
  object_ptr<callProtocol> protocol_;

  acceptCall();

  acceptCall(std::int32_t call_id_, object_ptr<callProtocol> &&protocol_);

  static const std::int32_t ID = -646618416;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class acceptTermsOfService final : public Function {
 public:
  std::string terms_of_service_id_;

  acceptTermsOfService();

  explicit acceptTermsOfService(std::string const &terms_of_service_id_);

  static const std::int32_t ID = 2130576356;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addChatMember final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;
  std::int32_t forward_limit_;

  addChatMember();

  addChatMember(std::int64_t chat_id_, std::int32_t user_id_, std::int32_t forward_limit_);

  static const std::int32_t ID = 1182817962;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addChatMembers final : public Function {
 public:
  std::int64_t chat_id_;
  std::vector<std::int32_t> user_ids_;

  addChatMembers();

  addChatMembers(std::int64_t chat_id_, std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = 1234094617;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addContact final : public Function {
 public:
  object_ptr<contact> contact_;
  bool share_phone_number_;

  addContact();

  addContact(object_ptr<contact> &&contact_, bool share_phone_number_);

  static const std::int32_t ID = 1869640000;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addCustomServerLanguagePack final : public Function {
 public:
  std::string language_pack_id_;

  addCustomServerLanguagePack();

  explicit addCustomServerLanguagePack(std::string const &language_pack_id_);

  static const std::int32_t ID = 4492771;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addFavoriteSticker final : public Function {
 public:
  object_ptr<InputFile> sticker_;

  addFavoriteSticker();

  explicit addFavoriteSticker(object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = 324504799;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addLocalMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t sender_user_id_;
  std::int64_t reply_to_message_id_;
  bool disable_notification_;
  object_ptr<InputMessageContent> input_message_content_;

  addLocalMessage();

  addLocalMessage(std::int64_t chat_id_, std::int32_t sender_user_id_, std::int64_t reply_to_message_id_, bool disable_notification_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -348943149;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addLogMessage final : public Function {
 public:
  std::int32_t verbosity_level_;
  std::string text_;

  addLogMessage();

  addLogMessage(std::int32_t verbosity_level_, std::string const &text_);

  static const std::int32_t ID = 1597427692;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addNetworkStatistics final : public Function {
 public:
  object_ptr<NetworkStatisticsEntry> entry_;

  addNetworkStatistics();

  explicit addNetworkStatistics(object_ptr<NetworkStatisticsEntry> &&entry_);

  static const std::int32_t ID = 1264825305;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addProxy final : public Function {
 public:
  std::string server_;
  std::int32_t port_;
  bool enable_;
  object_ptr<ProxyType> type_;

  addProxy();

  addProxy(std::string const &server_, std::int32_t port_, bool enable_, object_ptr<ProxyType> &&type_);

  static const std::int32_t ID = 331529432;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<proxy>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addRecentSticker final : public Function {
 public:
  bool is_attached_;
  object_ptr<InputFile> sticker_;

  addRecentSticker();

  addRecentSticker(bool is_attached_, object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = -1478109026;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addRecentlyFoundChat final : public Function {
 public:
  std::int64_t chat_id_;

  addRecentlyFoundChat();

  explicit addRecentlyFoundChat(std::int64_t chat_id_);

  static const std::int32_t ID = -1746396787;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addSavedAnimation final : public Function {
 public:
  object_ptr<InputFile> animation_;

  addSavedAnimation();

  explicit addSavedAnimation(object_ptr<InputFile> &&animation_);

  static const std::int32_t ID = -1538525088;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class addStickerToSet final : public Function {
 public:
  std::int32_t user_id_;
  std::string name_;
  object_ptr<inputSticker> sticker_;

  addStickerToSet();

  addStickerToSet(std::int32_t user_id_, std::string const &name_, object_ptr<inputSticker> &&sticker_);

  static const std::int32_t ID = 1422402800;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSet>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class answerCallbackQuery final : public Function {
 public:
  std::int64_t callback_query_id_;
  std::string text_;
  bool show_alert_;
  std::string url_;
  std::int32_t cache_time_;

  answerCallbackQuery();

  answerCallbackQuery(std::int64_t callback_query_id_, std::string const &text_, bool show_alert_, std::string const &url_, std::int32_t cache_time_);

  static const std::int32_t ID = -1153028490;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class answerCustomQuery final : public Function {
 public:
  std::int64_t custom_query_id_;
  std::string data_;

  answerCustomQuery();

  answerCustomQuery(std::int64_t custom_query_id_, std::string const &data_);

  static const std::int32_t ID = -1293603521;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class answerInlineQuery final : public Function {
 public:
  std::int64_t inline_query_id_;
  bool is_personal_;
  std::vector<object_ptr<InputInlineQueryResult>> results_;
  std::int32_t cache_time_;
  std::string next_offset_;
  std::string switch_pm_text_;
  std::string switch_pm_parameter_;

  answerInlineQuery();

  answerInlineQuery(std::int64_t inline_query_id_, bool is_personal_, std::vector<object_ptr<InputInlineQueryResult>> &&results_, std::int32_t cache_time_, std::string const &next_offset_, std::string const &switch_pm_text_, std::string const &switch_pm_parameter_);

  static const std::int32_t ID = 485879477;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class answerPreCheckoutQuery final : public Function {
 public:
  std::int64_t pre_checkout_query_id_;
  std::string error_message_;

  answerPreCheckoutQuery();

  answerPreCheckoutQuery(std::int64_t pre_checkout_query_id_, std::string const &error_message_);

  static const std::int32_t ID = -1486789653;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class answerShippingQuery final : public Function {
 public:
  std::int64_t shipping_query_id_;
  std::vector<object_ptr<shippingOption>> shipping_options_;
  std::string error_message_;

  answerShippingQuery();

  answerShippingQuery(std::int64_t shipping_query_id_, std::vector<object_ptr<shippingOption>> &&shipping_options_, std::string const &error_message_);

  static const std::int32_t ID = -434601324;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class blockUser final : public Function {
 public:
  std::int32_t user_id_;

  blockUser();

  explicit blockUser(std::int32_t user_id_);

  static const std::int32_t ID = -1239315139;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class canTransferOwnership final : public Function {
 public:

  canTransferOwnership();

  static const std::int32_t ID = 634602508;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<CanTransferOwnershipResult>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class cancelDownloadFile final : public Function {
 public:
  std::int32_t file_id_;
  bool only_if_pending_;

  cancelDownloadFile();

  cancelDownloadFile(std::int32_t file_id_, bool only_if_pending_);

  static const std::int32_t ID = -1954524450;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class cancelUploadFile final : public Function {
 public:
  std::int32_t file_id_;

  cancelUploadFile();

  explicit cancelUploadFile(std::int32_t file_id_);

  static const std::int32_t ID = 1623539600;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class changeImportedContacts final : public Function {
 public:
  std::vector<object_ptr<contact>> contacts_;

  changeImportedContacts();

  explicit changeImportedContacts(std::vector<object_ptr<contact>> &&contacts_);

  static const std::int32_t ID = 1968207955;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<importedContacts>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class changePhoneNumber final : public Function {
 public:
  std::string phone_number_;
  object_ptr<phoneNumberAuthenticationSettings> settings_;

  changePhoneNumber();

  changePhoneNumber(std::string const &phone_number_, object_ptr<phoneNumberAuthenticationSettings> &&settings_);

  static const std::int32_t ID = -124666973;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class changeStickerSet final : public Function {
 public:
  std::int64_t set_id_;
  bool is_installed_;
  bool is_archived_;

  changeStickerSet();

  changeStickerSet(std::int64_t set_id_, bool is_installed_, bool is_archived_);

  static const std::int32_t ID = 449357293;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkAuthenticationBotToken final : public Function {
 public:
  std::string token_;

  checkAuthenticationBotToken();

  explicit checkAuthenticationBotToken(std::string const &token_);

  static const std::int32_t ID = 639321206;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkAuthenticationCode final : public Function {
 public:
  std::string code_;

  checkAuthenticationCode();

  explicit checkAuthenticationCode(std::string const &code_);

  static const std::int32_t ID = -302103382;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkAuthenticationPassword final : public Function {
 public:
  std::string password_;

  checkAuthenticationPassword();

  explicit checkAuthenticationPassword(std::string const &password_);

  static const std::int32_t ID = -2025698400;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChangePhoneNumberCode final : public Function {
 public:
  std::string code_;

  checkChangePhoneNumberCode();

  explicit checkChangePhoneNumberCode(std::string const &code_);

  static const std::int32_t ID = -1720278429;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatInviteLink final : public Function {
 public:
  std::string invite_link_;

  checkChatInviteLink();

  explicit checkChatInviteLink(std::string const &invite_link_);

  static const std::int32_t ID = -496940997;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatInviteLinkInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkChatUsername final : public Function {
 public:
  std::int64_t chat_id_;
  std::string username_;

  checkChatUsername();

  checkChatUsername(std::int64_t chat_id_, std::string const &username_);

  static const std::int32_t ID = -119119344;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<CheckChatUsernameResult>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkCreatedPublicChatsLimit final : public Function {
 public:
  object_ptr<PublicChatType> type_;

  checkCreatedPublicChatsLimit();

  explicit checkCreatedPublicChatsLimit(object_ptr<PublicChatType> &&type_);

  static const std::int32_t ID = -445546591;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkDatabaseEncryptionKey final : public Function {
 public:
  std::string encryption_key_;

  checkDatabaseEncryptionKey();

  explicit checkDatabaseEncryptionKey(std::string const &encryption_key_);

  static const std::int32_t ID = 1018769307;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkEmailAddressVerificationCode final : public Function {
 public:
  std::string code_;

  checkEmailAddressVerificationCode();

  explicit checkEmailAddressVerificationCode(std::string const &code_);

  static const std::int32_t ID = -426386685;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkPhoneNumberConfirmationCode final : public Function {
 public:
  std::string code_;

  checkPhoneNumberConfirmationCode();

  explicit checkPhoneNumberConfirmationCode(std::string const &code_);

  static const std::int32_t ID = -1348060966;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkPhoneNumberVerificationCode final : public Function {
 public:
  std::string code_;

  checkPhoneNumberVerificationCode();

  explicit checkPhoneNumberVerificationCode(std::string const &code_);

  static const std::int32_t ID = 1497462718;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class checkRecoveryEmailAddressCode final : public Function {
 public:
  std::string code_;

  checkRecoveryEmailAddressCode();

  explicit checkRecoveryEmailAddressCode(std::string const &code_);

  static const std::int32_t ID = -1997039589;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class cleanFileName final : public Function {
 public:
  std::string file_name_;

  cleanFileName();

  explicit cleanFileName(std::string const &file_name_);

  static const std::int32_t ID = 967964667;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class clearAllDraftMessages final : public Function {
 public:
  bool exclude_secret_chats_;

  clearAllDraftMessages();

  explicit clearAllDraftMessages(bool exclude_secret_chats_);

  static const std::int32_t ID = -46369573;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class clearImportedContacts final : public Function {
 public:

  clearImportedContacts();

  static const std::int32_t ID = 869503298;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class clearRecentStickers final : public Function {
 public:
  bool is_attached_;

  clearRecentStickers();

  explicit clearRecentStickers(bool is_attached_);

  static const std::int32_t ID = -321242684;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class clearRecentlyFoundChats final : public Function {
 public:

  clearRecentlyFoundChats();

  static const std::int32_t ID = -285582542;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class close final : public Function {
 public:

  close();

  static const std::int32_t ID = -1187782273;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class closeChat final : public Function {
 public:
  std::int64_t chat_id_;

  closeChat();

  explicit closeChat(std::int64_t chat_id_);

  static const std::int32_t ID = 39749353;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class closeSecretChat final : public Function {
 public:
  std::int32_t secret_chat_id_;

  closeSecretChat();

  explicit closeSecretChat(std::int32_t secret_chat_id_);

  static const std::int32_t ID = -471006133;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class confirmQrCodeAuthentication final : public Function {
 public:
  std::string link_;

  confirmQrCodeAuthentication();

  explicit confirmQrCodeAuthentication(std::string const &link_);

  static const std::int32_t ID = -376199379;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<session>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createBasicGroupChat final : public Function {
 public:
  std::int32_t basic_group_id_;
  bool force_;

  createBasicGroupChat();

  createBasicGroupChat(std::int32_t basic_group_id_, bool force_);

  static const std::int32_t ID = 642492777;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createCall final : public Function {
 public:
  std::int32_t user_id_;
  object_ptr<callProtocol> protocol_;

  createCall();

  createCall(std::int32_t user_id_, object_ptr<callProtocol> &&protocol_);

  static const std::int32_t ID = -1742408159;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<callId>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createNewBasicGroupChat final : public Function {
 public:
  std::vector<std::int32_t> user_ids_;
  std::string title_;

  createNewBasicGroupChat();

  createNewBasicGroupChat(std::vector<std::int32_t> &&user_ids_, std::string const &title_);

  static const std::int32_t ID = 1874532069;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createNewSecretChat final : public Function {
 public:
  std::int32_t user_id_;

  createNewSecretChat();

  explicit createNewSecretChat(std::int32_t user_id_);

  static const std::int32_t ID = 1689344881;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createNewStickerSet final : public Function {
 public:
  std::int32_t user_id_;
  std::string title_;
  std::string name_;
  bool is_masks_;
  std::vector<object_ptr<inputSticker>> stickers_;

  createNewStickerSet();

  createNewStickerSet(std::int32_t user_id_, std::string const &title_, std::string const &name_, bool is_masks_, std::vector<object_ptr<inputSticker>> &&stickers_);

  static const std::int32_t ID = 205093058;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSet>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createNewSupergroupChat final : public Function {
 public:
  std::string title_;
  bool is_channel_;
  std::string description_;
  object_ptr<chatLocation> location_;

  createNewSupergroupChat();

  createNewSupergroupChat(std::string const &title_, bool is_channel_, std::string const &description_, object_ptr<chatLocation> &&location_);

  static const std::int32_t ID = -8999251;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createPrivateChat final : public Function {
 public:
  std::int32_t user_id_;
  bool force_;

  createPrivateChat();

  createPrivateChat(std::int32_t user_id_, bool force_);

  static const std::int32_t ID = -1807530364;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createSecretChat final : public Function {
 public:
  std::int32_t secret_chat_id_;

  createSecretChat();

  explicit createSecretChat(std::int32_t secret_chat_id_);

  static const std::int32_t ID = 1930285615;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createSupergroupChat final : public Function {
 public:
  std::int32_t supergroup_id_;
  bool force_;

  createSupergroupChat();

  createSupergroupChat(std::int32_t supergroup_id_, bool force_);

  static const std::int32_t ID = 352742758;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class createTemporaryPassword final : public Function {
 public:
  std::string password_;
  std::int32_t valid_for_;

  createTemporaryPassword();

  createTemporaryPassword(std::string const &password_, std::int32_t valid_for_);

  static const std::int32_t ID = -1626509434;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<temporaryPasswordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteAccount final : public Function {
 public:
  std::string reason_;

  deleteAccount();

  explicit deleteAccount(std::string const &reason_);

  static const std::int32_t ID = -1203056508;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteChatHistory final : public Function {
 public:
  std::int64_t chat_id_;
  bool remove_from_chat_list_;
  bool revoke_;

  deleteChatHistory();

  deleteChatHistory(std::int64_t chat_id_, bool remove_from_chat_list_, bool revoke_);

  static const std::int32_t ID = -1472081761;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteChatMessagesFromUser final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;

  deleteChatMessagesFromUser();

  deleteChatMessagesFromUser(std::int64_t chat_id_, std::int32_t user_id_);

  static const std::int32_t ID = -1599689199;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteChatReplyMarkup final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  deleteChatReplyMarkup();

  deleteChatReplyMarkup(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = 100637531;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteFile final : public Function {
 public:
  std::int32_t file_id_;

  deleteFile();

  explicit deleteFile(std::int32_t file_id_);

  static const std::int32_t ID = 1807653676;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteLanguagePack final : public Function {
 public:
  std::string language_pack_id_;

  deleteLanguagePack();

  explicit deleteLanguagePack(std::string const &language_pack_id_);

  static const std::int32_t ID = -2108761026;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::vector<std::int64_t> message_ids_;
  bool revoke_;

  deleteMessages();

  deleteMessages(std::int64_t chat_id_, std::vector<std::int64_t> &&message_ids_, bool revoke_);

  static const std::int32_t ID = 1130090173;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deletePassportElement final : public Function {
 public:
  object_ptr<PassportElementType> type_;

  deletePassportElement();

  explicit deletePassportElement(object_ptr<PassportElementType> &&type_);

  static const std::int32_t ID = -1719555468;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteProfilePhoto final : public Function {
 public:
  std::int64_t profile_photo_id_;

  deleteProfilePhoto();

  explicit deleteProfilePhoto(std::int64_t profile_photo_id_);

  static const std::int32_t ID = 1319794625;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteSavedCredentials final : public Function {
 public:

  deleteSavedCredentials();

  static const std::int32_t ID = 826300114;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteSavedOrderInfo final : public Function {
 public:

  deleteSavedOrderInfo();

  static const std::int32_t ID = 1629058164;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class deleteSupergroup final : public Function {
 public:
  std::int32_t supergroup_id_;

  deleteSupergroup();

  explicit deleteSupergroup(std::int32_t supergroup_id_);

  static const std::int32_t ID = -1999855965;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class destroy final : public Function {
 public:

  destroy();

  static const std::int32_t ID = 685331274;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class disableProxy final : public Function {
 public:

  disableProxy();

  static const std::int32_t ID = -2100095102;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class discardCall final : public Function {
 public:
  std::int32_t call_id_;
  bool is_disconnected_;
  std::int32_t duration_;
  std::int64_t connection_id_;

  discardCall();

  discardCall(std::int32_t call_id_, bool is_disconnected_, std::int32_t duration_, std::int64_t connection_id_);

  static const std::int32_t ID = -923187372;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class disconnectAllWebsites final : public Function {
 public:

  disconnectAllWebsites();

  static const std::int32_t ID = -1082985981;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class disconnectWebsite final : public Function {
 public:
  std::int64_t website_id_;

  disconnectWebsite();

  explicit disconnectWebsite(std::int64_t website_id_);

  static const std::int32_t ID = -778767395;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class downloadFile final : public Function {
 public:
  std::int32_t file_id_;
  std::int32_t priority_;
  std::int32_t offset_;
  std::int32_t limit_;
  bool synchronous_;

  downloadFile();

  downloadFile(std::int32_t file_id_, std::int32_t priority_, std::int32_t offset_, std::int32_t limit_, bool synchronous_);

  static const std::int32_t ID = -1102026662;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editCustomLanguagePackInfo final : public Function {
 public:
  object_ptr<languagePackInfo> info_;

  editCustomLanguagePackInfo();

  explicit editCustomLanguagePackInfo(object_ptr<languagePackInfo> &&info_);

  static const std::int32_t ID = 1320751257;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editInlineMessageCaption final : public Function {
 public:
  std::string inline_message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<formattedText> caption_;

  editInlineMessageCaption();

  editInlineMessageCaption(std::string const &inline_message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = -760985929;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editInlineMessageLiveLocation final : public Function {
 public:
  std::string inline_message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<location> location_;

  editInlineMessageLiveLocation();

  editInlineMessageLiveLocation(std::string const &inline_message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<location> &&location_);

  static const std::int32_t ID = 655046316;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editInlineMessageMedia final : public Function {
 public:
  std::string inline_message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  editInlineMessageMedia();

  editInlineMessageMedia(std::string const &inline_message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 23553921;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editInlineMessageReplyMarkup final : public Function {
 public:
  std::string inline_message_id_;
  object_ptr<ReplyMarkup> reply_markup_;

  editInlineMessageReplyMarkup();

  editInlineMessageReplyMarkup(std::string const &inline_message_id_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = -67565858;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editInlineMessageText final : public Function {
 public:
  std::string inline_message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  editInlineMessageText();

  editInlineMessageText(std::string const &inline_message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -855457307;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageCaption final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<formattedText> caption_;

  editMessageCaption();

  editMessageCaption(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<formattedText> &&caption_);

  static const std::int32_t ID = 1154677038;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageLiveLocation final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<location> location_;

  editMessageLiveLocation();

  editMessageLiveLocation(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<location> &&location_);

  static const std::int32_t ID = -1146772745;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageMedia final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  editMessageMedia();

  editMessageMedia(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1152678125;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageReplyMarkup final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;

  editMessageReplyMarkup();

  editMessageReplyMarkup(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = 332127881;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageSchedulingState final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<MessageSchedulingState> scheduling_state_;

  editMessageSchedulingState();

  editMessageSchedulingState(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<MessageSchedulingState> &&scheduling_state_);

  static const std::int32_t ID = -1372976192;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editMessageText final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  editMessageText();

  editMessageText(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = 196272567;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class editProxy final : public Function {
 public:
  std::int32_t proxy_id_;
  std::string server_;
  std::int32_t port_;
  bool enable_;
  object_ptr<ProxyType> type_;

  editProxy();

  editProxy(std::int32_t proxy_id_, std::string const &server_, std::int32_t port_, bool enable_, object_ptr<ProxyType> &&type_);

  static const std::int32_t ID = -1605883821;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<proxy>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class enableProxy final : public Function {
 public:
  std::int32_t proxy_id_;

  enableProxy();

  explicit enableProxy(std::int32_t proxy_id_);

  static const std::int32_t ID = 1494450838;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class finishFileGeneration final : public Function {
 public:
  std::int64_t generation_id_;
  object_ptr<error> error_;

  finishFileGeneration();

  finishFileGeneration(std::int64_t generation_id_, object_ptr<error> &&error_);

  static const std::int32_t ID = -1055060835;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class forwardMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t from_chat_id_;
  std::vector<std::int64_t> message_ids_;
  object_ptr<sendMessageOptions> options_;
  bool as_album_;
  bool send_copy_;
  bool remove_caption_;

  forwardMessages();

  forwardMessages(std::int64_t chat_id_, std::int64_t from_chat_id_, std::vector<std::int64_t> &&message_ids_, object_ptr<sendMessageOptions> &&options_, bool as_album_, bool send_copy_, bool remove_caption_);

  static const std::int32_t ID = -1633531094;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class generateChatInviteLink final : public Function {
 public:
  std::int64_t chat_id_;

  generateChatInviteLink();

  explicit generateChatInviteLink(std::int64_t chat_id_);

  static const std::int32_t ID = 1945532500;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatInviteLink>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getAccountTtl final : public Function {
 public:

  getAccountTtl();

  static const std::int32_t ID = -443905161;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<accountTtl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getActiveLiveLocationMessages final : public Function {
 public:

  getActiveLiveLocationMessages();

  static const std::int32_t ID = -1425459567;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getActiveSessions final : public Function {
 public:

  getActiveSessions();

  static const std::int32_t ID = 1119710526;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<sessions>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getAllPassportElements final : public Function {
 public:
  std::string password_;

  getAllPassportElements();

  explicit getAllPassportElements(std::string const &password_);

  static const std::int32_t ID = -2038945045;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passportElements>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getApplicationConfig final : public Function {
 public:

  getApplicationConfig();

  static const std::int32_t ID = -1823144318;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<JsonValue>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getArchivedStickerSets final : public Function {
 public:
  bool is_masks_;
  std::int64_t offset_sticker_set_id_;
  std::int32_t limit_;

  getArchivedStickerSets();

  getArchivedStickerSets(bool is_masks_, std::int64_t offset_sticker_set_id_, std::int32_t limit_);

  static const std::int32_t ID = 1996943238;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getAttachedStickerSets final : public Function {
 public:
  std::int32_t file_id_;

  getAttachedStickerSets();

  explicit getAttachedStickerSets(std::int32_t file_id_);

  static const std::int32_t ID = 1302172429;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getAuthorizationState final : public Function {
 public:

  getAuthorizationState();

  static const std::int32_t ID = 1949154877;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<AuthorizationState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getAutoDownloadSettingsPresets final : public Function {
 public:

  getAutoDownloadSettingsPresets();

  static const std::int32_t ID = -1721088201;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<autoDownloadSettingsPresets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getBackgroundUrl final : public Function {
 public:
  std::string name_;
  object_ptr<BackgroundType> type_;

  getBackgroundUrl();

  getBackgroundUrl(std::string const &name_, object_ptr<BackgroundType> &&type_);

  static const std::int32_t ID = 733769682;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<httpUrl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getBackgrounds final : public Function {
 public:
  bool for_dark_theme_;

  getBackgrounds();

  explicit getBackgrounds(bool for_dark_theme_);

  static const std::int32_t ID = 249072633;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<backgrounds>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getBasicGroup final : public Function {
 public:
  std::int32_t basic_group_id_;

  getBasicGroup();

  explicit getBasicGroup(std::int32_t basic_group_id_);

  static const std::int32_t ID = 561775568;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<basicGroup>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getBasicGroupFullInfo final : public Function {
 public:
  std::int32_t basic_group_id_;

  getBasicGroupFullInfo();

  explicit getBasicGroupFullInfo(std::int32_t basic_group_id_);

  static const std::int32_t ID = 1770517905;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<basicGroupFullInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getBlockedUsers final : public Function {
 public:
  std::int32_t offset_;
  std::int32_t limit_;

  getBlockedUsers();

  getBlockedUsers(std::int32_t offset_, std::int32_t limit_);

  static const std::int32_t ID = -742912777;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<users>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getCallbackQueryAnswer final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<CallbackQueryPayload> payload_;

  getCallbackQueryAnswer();

  getCallbackQueryAnswer(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<CallbackQueryPayload> &&payload_);

  static const std::int32_t ID = 116357727;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<callbackQueryAnswer>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChat final : public Function {
 public:
  std::int64_t chat_id_;

  getChat();

  explicit getChat(std::int64_t chat_id_);

  static const std::int32_t ID = 1866601536;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatAdministrators final : public Function {
 public:
  std::int64_t chat_id_;

  getChatAdministrators();

  explicit getChatAdministrators(std::int64_t chat_id_);

  static const std::int32_t ID = 1544468155;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatAdministrators>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatEventLog final : public Function {
 public:
  std::int64_t chat_id_;
  std::string query_;
  std::int64_t from_event_id_;
  std::int32_t limit_;
  object_ptr<chatEventLogFilters> filters_;
  std::vector<std::int32_t> user_ids_;

  getChatEventLog();

  getChatEventLog(std::int64_t chat_id_, std::string const &query_, std::int64_t from_event_id_, std::int32_t limit_, object_ptr<chatEventLogFilters> &&filters_, std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = 206900967;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatEvents>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatHistory final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t from_message_id_;
  std::int32_t offset_;
  std::int32_t limit_;
  bool only_local_;

  getChatHistory();

  getChatHistory(std::int64_t chat_id_, std::int64_t from_message_id_, std::int32_t offset_, std::int32_t limit_, bool only_local_);

  static const std::int32_t ID = -799960451;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatMember final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;

  getChatMember();

  getChatMember(std::int64_t chat_id_, std::int32_t user_id_);

  static const std::int32_t ID = 677085892;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatMember>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatMessageByDate final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t date_;

  getChatMessageByDate();

  getChatMessageByDate(std::int64_t chat_id_, std::int32_t date_);

  static const std::int32_t ID = 1062564150;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatMessageCount final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<SearchMessagesFilter> filter_;
  bool return_local_;

  getChatMessageCount();

  getChatMessageCount(std::int64_t chat_id_, object_ptr<SearchMessagesFilter> &&filter_, bool return_local_);

  static const std::int32_t ID = 205435308;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<count>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatNotificationSettingsExceptions final : public Function {
 public:
  object_ptr<NotificationSettingsScope> scope_;
  bool compare_sound_;

  getChatNotificationSettingsExceptions();

  getChatNotificationSettingsExceptions(object_ptr<NotificationSettingsScope> &&scope_, bool compare_sound_);

  static const std::int32_t ID = 201199121;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatPinnedMessage final : public Function {
 public:
  std::int64_t chat_id_;

  getChatPinnedMessage();

  explicit getChatPinnedMessage(std::int64_t chat_id_);

  static const std::int32_t ID = 359865008;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatScheduledMessages final : public Function {
 public:
  std::int64_t chat_id_;

  getChatScheduledMessages();

  explicit getChatScheduledMessages(std::int64_t chat_id_);

  static const std::int32_t ID = -549638149;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChatStatisticsUrl final : public Function {
 public:
  std::int64_t chat_id_;
  std::string parameters_;
  bool is_dark_;

  getChatStatisticsUrl();

  getChatStatisticsUrl(std::int64_t chat_id_, std::string const &parameters_, bool is_dark_);

  static const std::int32_t ID = 1114621183;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<httpUrl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getChats final : public Function {
 public:
  object_ptr<ChatList> chat_list_;
  std::int64_t offset_order_;
  std::int64_t offset_chat_id_;
  std::int32_t limit_;

  getChats();

  getChats(object_ptr<ChatList> &&chat_list_, std::int64_t offset_order_, std::int64_t offset_chat_id_, std::int32_t limit_);

  static const std::int32_t ID = 1847129537;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getConnectedWebsites final : public Function {
 public:

  getConnectedWebsites();

  static const std::int32_t ID = -170536110;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<connectedWebsites>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getContacts final : public Function {
 public:

  getContacts();

  static const std::int32_t ID = -1417722768;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<users>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getCountryCode final : public Function {
 public:

  getCountryCode();

  static const std::int32_t ID = 1540593906;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getCreatedPublicChats final : public Function {
 public:
  object_ptr<PublicChatType> type_;

  getCreatedPublicChats();

  explicit getCreatedPublicChats(object_ptr<PublicChatType> &&type_);

  static const std::int32_t ID = 710354415;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getCurrentState final : public Function {
 public:

  getCurrentState();

  static const std::int32_t ID = -1191417719;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<updates>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getDatabaseStatistics final : public Function {
 public:

  getDatabaseStatistics();

  static const std::int32_t ID = -1942760263;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<databaseStatistics>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getDeepLinkInfo final : public Function {
 public:
  std::string link_;

  getDeepLinkInfo();

  explicit getDeepLinkInfo(std::string const &link_);

  static const std::int32_t ID = 680673150;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<deepLinkInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getEmojiSuggestionsUrl final : public Function {
 public:
  std::string language_code_;

  getEmojiSuggestionsUrl();

  explicit getEmojiSuggestionsUrl(std::string const &language_code_);

  static const std::int32_t ID = -1404101841;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<httpUrl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getFavoriteStickers final : public Function {
 public:

  getFavoriteStickers();

  static const std::int32_t ID = -338964672;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getFile final : public Function {
 public:
  std::int32_t file_id_;

  getFile();

  explicit getFile(std::int32_t file_id_);

  static const std::int32_t ID = 1553923406;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getFileDownloadedPrefixSize final : public Function {
 public:
  std::int32_t file_id_;
  std::int32_t offset_;

  getFileDownloadedPrefixSize();

  getFileDownloadedPrefixSize(std::int32_t file_id_, std::int32_t offset_);

  static const std::int32_t ID = -1668864864;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<count>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getFileExtension final : public Function {
 public:
  std::string mime_type_;

  getFileExtension();

  explicit getFileExtension(std::string const &mime_type_);

  static const std::int32_t ID = -106055372;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getFileMimeType final : public Function {
 public:
  std::string file_name_;

  getFileMimeType();

  explicit getFileMimeType(std::string const &file_name_);

  static const std::int32_t ID = -2073879671;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getGameHighScores final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t user_id_;

  getGameHighScores();

  getGameHighScores(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t user_id_);

  static const std::int32_t ID = 1920923753;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<gameHighScores>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getGroupsInCommon final : public Function {
 public:
  std::int32_t user_id_;
  std::int64_t offset_chat_id_;
  std::int32_t limit_;

  getGroupsInCommon();

  getGroupsInCommon(std::int32_t user_id_, std::int64_t offset_chat_id_, std::int32_t limit_);

  static const std::int32_t ID = -23238689;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getImportedContactCount final : public Function {
 public:

  getImportedContactCount();

  static const std::int32_t ID = -656336346;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<count>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getInactiveSupergroupChats final : public Function {
 public:

  getInactiveSupergroupChats();

  static const std::int32_t ID = -657720907;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getInlineGameHighScores final : public Function {
 public:
  std::string inline_message_id_;
  std::int32_t user_id_;

  getInlineGameHighScores();

  getInlineGameHighScores(std::string const &inline_message_id_, std::int32_t user_id_);

  static const std::int32_t ID = -1833445800;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<gameHighScores>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getInlineQueryResults final : public Function {
 public:
  std::int32_t bot_user_id_;
  std::int64_t chat_id_;
  object_ptr<location> user_location_;
  std::string query_;
  std::string offset_;

  getInlineQueryResults();

  getInlineQueryResults(std::int32_t bot_user_id_, std::int64_t chat_id_, object_ptr<location> &&user_location_, std::string const &query_, std::string const &offset_);

  static const std::int32_t ID = -1182511172;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<inlineQueryResults>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getInstalledStickerSets final : public Function {
 public:
  bool is_masks_;

  getInstalledStickerSets();

  explicit getInstalledStickerSets(bool is_masks_);

  static const std::int32_t ID = 1214523749;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getInviteText final : public Function {
 public:

  getInviteText();

  static const std::int32_t ID = 794573512;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getJsonString final : public Function {
 public:
  object_ptr<JsonValue> json_value_;

  getJsonString();

  explicit getJsonString(object_ptr<JsonValue> &&json_value_);

  static const std::int32_t ID = 663458849;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getJsonValue final : public Function {
 public:
  std::string json_;

  getJsonValue();

  explicit getJsonValue(std::string const &json_);

  static const std::int32_t ID = -1829086715;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<JsonValue>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLanguagePackInfo final : public Function {
 public:
  std::string language_pack_id_;

  getLanguagePackInfo();

  explicit getLanguagePackInfo(std::string const &language_pack_id_);

  static const std::int32_t ID = 2077809320;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<languagePackInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLanguagePackString final : public Function {
 public:
  std::string language_pack_database_path_;
  std::string localization_target_;
  std::string language_pack_id_;
  std::string key_;

  getLanguagePackString();

  getLanguagePackString(std::string const &language_pack_database_path_, std::string const &localization_target_, std::string const &language_pack_id_, std::string const &key_);

  static const std::int32_t ID = 150789747;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<LanguagePackStringValue>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLanguagePackStrings final : public Function {
 public:
  std::string language_pack_id_;
  std::vector<std::string> keys_;

  getLanguagePackStrings();

  getLanguagePackStrings(std::string const &language_pack_id_, std::vector<std::string> &&keys_);

  static const std::int32_t ID = 1246259088;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<languagePackStrings>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLocalizationTargetInfo final : public Function {
 public:
  bool only_local_;

  getLocalizationTargetInfo();

  explicit getLocalizationTargetInfo(bool only_local_);

  static const std::int32_t ID = 1849499526;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<localizationTargetInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLogStream final : public Function {
 public:

  getLogStream();

  static const std::int32_t ID = 1167608667;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<LogStream>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLogTagVerbosityLevel final : public Function {
 public:
  std::string tag_;

  getLogTagVerbosityLevel();

  explicit getLogTagVerbosityLevel(std::string const &tag_);

  static const std::int32_t ID = 951004547;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<logVerbosityLevel>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLogTags final : public Function {
 public:

  getLogTags();

  static const std::int32_t ID = -254449190;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<logTags>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLogVerbosityLevel final : public Function {
 public:

  getLogVerbosityLevel();

  static const std::int32_t ID = 594057956;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<logVerbosityLevel>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLoginUrl final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t button_id_;
  bool allow_write_access_;

  getLoginUrl();

  getLoginUrl(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t button_id_, bool allow_write_access_);

  static const std::int32_t ID = 694973925;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<httpUrl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getLoginUrlInfo final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t button_id_;

  getLoginUrlInfo();

  getLoginUrlInfo(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t button_id_);

  static const std::int32_t ID = -980042966;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<LoginUrlInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMapThumbnailFile final : public Function {
 public:
  object_ptr<location> location_;
  std::int32_t zoom_;
  std::int32_t width_;
  std::int32_t height_;
  std::int32_t scale_;
  std::int64_t chat_id_;

  getMapThumbnailFile();

  getMapThumbnailFile(object_ptr<location> &&location_, std::int32_t zoom_, std::int32_t width_, std::int32_t height_, std::int32_t scale_, std::int64_t chat_id_);

  static const std::int32_t ID = -152660070;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMe final : public Function {
 public:

  getMe();

  static const std::int32_t ID = -191516033;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<user>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getMessage();

  getMessage(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -1821196160;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMessageLink final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getMessageLink();

  getMessageLink(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = 1362732326;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<httpUrl>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMessageLinkInfo final : public Function {
 public:
  std::string url_;

  getMessageLinkInfo();

  explicit getMessageLinkInfo(std::string const &url_);

  static const std::int32_t ID = -700533672;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messageLinkInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMessageLocally final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getMessageLocally();

  getMessageLocally(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -603575444;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::vector<std::int64_t> message_ids_;

  getMessages();

  getMessages(std::int64_t chat_id_, std::vector<std::int64_t> &&message_ids_);

  static const std::int32_t ID = 425299338;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getNetworkStatistics final : public Function {
 public:
  bool only_current_;

  getNetworkStatistics();

  explicit getNetworkStatistics(bool only_current_);

  static const std::int32_t ID = -986228706;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<networkStatistics>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getOption final : public Function {
 public:
  std::string name_;

  getOption();

  explicit getOption(std::string const &name_);

  static const std::int32_t ID = -1572495746;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<OptionValue>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPassportAuthorizationForm final : public Function {
 public:
  std::int32_t bot_user_id_;
  std::string scope_;
  std::string public_key_;
  std::string nonce_;

  getPassportAuthorizationForm();

  getPassportAuthorizationForm(std::int32_t bot_user_id_, std::string const &scope_, std::string const &public_key_, std::string const &nonce_);

  static const std::int32_t ID = -1468394095;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passportAuthorizationForm>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPassportAuthorizationFormAvailableElements final : public Function {
 public:
  std::int32_t autorization_form_id_;
  std::string password_;

  getPassportAuthorizationFormAvailableElements();

  getPassportAuthorizationFormAvailableElements(std::int32_t autorization_form_id_, std::string const &password_);

  static const std::int32_t ID = 1738134754;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passportElementsWithErrors>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPassportElement final : public Function {
 public:
  object_ptr<PassportElementType> type_;
  std::string password_;

  getPassportElement();

  getPassportElement(object_ptr<PassportElementType> &&type_, std::string const &password_);

  static const std::int32_t ID = -1882398342;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<PassportElement>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPasswordState final : public Function {
 public:

  getPasswordState();

  static const std::int32_t ID = -174752904;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPaymentForm final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getPaymentForm();

  getPaymentForm(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -2146950882;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<paymentForm>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPaymentReceipt final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getPaymentReceipt();

  getPaymentReceipt(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = 1013758294;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<paymentReceipt>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPollVoters final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::int32_t option_id_;
  std::int32_t offset_;
  std::int32_t limit_;

  getPollVoters();

  getPollVoters(std::int64_t chat_id_, std::int64_t message_id_, std::int32_t option_id_, std::int32_t offset_, std::int32_t limit_);

  static const std::int32_t ID = 2075288734;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<users>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPreferredCountryLanguage final : public Function {
 public:
  std::string country_code_;

  getPreferredCountryLanguage();

  explicit getPreferredCountryLanguage(std::string const &country_code_);

  static const std::int32_t ID = -933049386;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getProxies final : public Function {
 public:

  getProxies();

  static const std::int32_t ID = -95026381;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<proxies>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getProxyLink final : public Function {
 public:
  std::int32_t proxy_id_;

  getProxyLink();

  explicit getProxyLink(std::int32_t proxy_id_);

  static const std::int32_t ID = -1285597664;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<text>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPublicMessageLink final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  bool for_album_;

  getPublicMessageLink();

  getPublicMessageLink(std::int64_t chat_id_, std::int64_t message_id_, bool for_album_);

  static const std::int32_t ID = -374642839;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<publicMessageLink>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getPushReceiverId final : public Function {
 public:
  std::string payload_;

  getPushReceiverId();

  explicit getPushReceiverId(std::string const &payload_);

  static const std::int32_t ID = -286505294;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<pushReceiverId>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRecentInlineBots final : public Function {
 public:

  getRecentInlineBots();

  static const std::int32_t ID = 1437823548;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<users>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRecentStickers final : public Function {
 public:
  bool is_attached_;

  getRecentStickers();

  explicit getRecentStickers(bool is_attached_);

  static const std::int32_t ID = -579622241;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRecentlyVisitedTMeUrls final : public Function {
 public:
  std::string referrer_;

  getRecentlyVisitedTMeUrls();

  explicit getRecentlyVisitedTMeUrls(std::string const &referrer_);

  static const std::int32_t ID = 806754961;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<tMeUrls>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRecoveryEmailAddress final : public Function {
 public:
  std::string password_;

  getRecoveryEmailAddress();

  explicit getRecoveryEmailAddress(std::string const &password_);

  static const std::int32_t ID = -1594770947;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<recoveryEmailAddress>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRemoteFile final : public Function {
 public:
  std::string remote_file_id_;
  object_ptr<FileType> file_type_;

  getRemoteFile();

  getRemoteFile(std::string const &remote_file_id_, object_ptr<FileType> &&file_type_);

  static const std::int32_t ID = 2137204530;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getRepliedMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  getRepliedMessage();

  getRepliedMessage(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -641918531;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSavedAnimations final : public Function {
 public:

  getSavedAnimations();

  static const std::int32_t ID = 7051032;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<animations>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSavedOrderInfo final : public Function {
 public:

  getSavedOrderInfo();

  static const std::int32_t ID = -1152016675;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<orderInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getScopeNotificationSettings final : public Function {
 public:
  object_ptr<NotificationSettingsScope> scope_;

  getScopeNotificationSettings();

  explicit getScopeNotificationSettings(object_ptr<NotificationSettingsScope> &&scope_);

  static const std::int32_t ID = -995613361;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<scopeNotificationSettings>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSecretChat final : public Function {
 public:
  std::int32_t secret_chat_id_;

  getSecretChat();

  explicit getSecretChat(std::int32_t secret_chat_id_);

  static const std::int32_t ID = 40599169;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<secretChat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getStickerEmojis final : public Function {
 public:
  object_ptr<InputFile> sticker_;

  getStickerEmojis();

  explicit getStickerEmojis(object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = -1895508665;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<emojis>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getStickerSet final : public Function {
 public:
  std::int64_t set_id_;

  getStickerSet();

  explicit getStickerSet(std::int64_t set_id_);

  static const std::int32_t ID = 1052318659;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSet>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getStickers final : public Function {
 public:
  std::string emoji_;
  std::int32_t limit_;

  getStickers();

  getStickers(std::string const &emoji_, std::int32_t limit_);

  static const std::int32_t ID = -1594919556;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getStorageStatistics final : public Function {
 public:
  std::int32_t chat_limit_;

  getStorageStatistics();

  explicit getStorageStatistics(std::int32_t chat_limit_);

  static const std::int32_t ID = -853193929;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<storageStatistics>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getStorageStatisticsFast final : public Function {
 public:

  getStorageStatisticsFast();

  static const std::int32_t ID = 61368066;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<storageStatisticsFast>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSuitableDiscussionChats final : public Function {
 public:

  getSuitableDiscussionChats();

  static const std::int32_t ID = 49044982;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSupergroup final : public Function {
 public:
  std::int32_t supergroup_id_;

  getSupergroup();

  explicit getSupergroup(std::int32_t supergroup_id_);

  static const std::int32_t ID = -2063063706;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<supergroup>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSupergroupFullInfo final : public Function {
 public:
  std::int32_t supergroup_id_;

  getSupergroupFullInfo();

  explicit getSupergroupFullInfo(std::int32_t supergroup_id_);

  static const std::int32_t ID = -1150331262;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<supergroupFullInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSupergroupMembers final : public Function {
 public:
  std::int32_t supergroup_id_;
  object_ptr<SupergroupMembersFilter> filter_;
  std::int32_t offset_;
  std::int32_t limit_;

  getSupergroupMembers();

  getSupergroupMembers(std::int32_t supergroup_id_, object_ptr<SupergroupMembersFilter> &&filter_, std::int32_t offset_, std::int32_t limit_);

  static const std::int32_t ID = 1427643098;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatMembers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getSupportUser final : public Function {
 public:

  getSupportUser();

  static const std::int32_t ID = -1733497700;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<user>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getTemporaryPasswordState final : public Function {
 public:

  getTemporaryPasswordState();

  static const std::int32_t ID = -12670830;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<temporaryPasswordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getTextEntities final : public Function {
 public:
  std::string text_;

  getTextEntities();

  explicit getTextEntities(std::string const &text_);

  static const std::int32_t ID = -341490693;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<textEntities>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getTopChats final : public Function {
 public:
  object_ptr<TopChatCategory> category_;
  std::int32_t limit_;

  getTopChats();

  getTopChats(object_ptr<TopChatCategory> &&category_, std::int32_t limit_);

  static const std::int32_t ID = -388410847;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getTrendingStickerSets final : public Function {
 public:

  getTrendingStickerSets();

  static const std::int32_t ID = -1729129957;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getUser final : public Function {
 public:
  std::int32_t user_id_;

  getUser();

  explicit getUser(std::int32_t user_id_);

  static const std::int32_t ID = -47586017;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<user>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getUserFullInfo final : public Function {
 public:
  std::int32_t user_id_;

  getUserFullInfo();

  explicit getUserFullInfo(std::int32_t user_id_);

  static const std::int32_t ID = -655443263;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<userFullInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getUserPrivacySettingRules final : public Function {
 public:
  object_ptr<UserPrivacySetting> setting_;

  getUserPrivacySettingRules();

  explicit getUserPrivacySettingRules(object_ptr<UserPrivacySetting> &&setting_);

  static const std::int32_t ID = -2077223311;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<userPrivacySettingRules>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getUserProfilePhotos final : public Function {
 public:
  std::int32_t user_id_;
  std::int32_t offset_;
  std::int32_t limit_;

  getUserProfilePhotos();

  getUserProfilePhotos(std::int32_t user_id_, std::int32_t offset_, std::int32_t limit_);

  static const std::int32_t ID = -2062927433;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<userProfilePhotos>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getWebPageInstantView final : public Function {
 public:
  std::string url_;
  bool force_full_;

  getWebPageInstantView();

  getWebPageInstantView(std::string const &url_, bool force_full_);

  static const std::int32_t ID = -1962649975;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<webPageInstantView>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class getWebPagePreview final : public Function {
 public:
  object_ptr<formattedText> text_;

  getWebPagePreview();

  explicit getWebPagePreview(object_ptr<formattedText> &&text_);

  static const std::int32_t ID = 573441580;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<webPage>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class importContacts final : public Function {
 public:
  std::vector<object_ptr<contact>> contacts_;

  importContacts();

  explicit importContacts(std::vector<object_ptr<contact>> &&contacts_);

  static const std::int32_t ID = -215132767;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<importedContacts>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class joinChat final : public Function {
 public:
  std::int64_t chat_id_;

  joinChat();

  explicit joinChat(std::int64_t chat_id_);

  static const std::int32_t ID = 326769313;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class joinChatByInviteLink final : public Function {
 public:
  std::string invite_link_;

  joinChatByInviteLink();

  explicit joinChatByInviteLink(std::string const &invite_link_);

  static const std::int32_t ID = -1049973882;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class leaveChat final : public Function {
 public:
  std::int64_t chat_id_;

  leaveChat();

  explicit leaveChat(std::int64_t chat_id_);

  static const std::int32_t ID = -1825080735;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class logOut final : public Function {
 public:

  logOut();

  static const std::int32_t ID = -1581923301;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class openChat final : public Function {
 public:
  std::int64_t chat_id_;

  openChat();

  explicit openChat(std::int64_t chat_id_);

  static const std::int32_t ID = -323371509;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class openMessageContent final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;

  openMessageContent();

  openMessageContent(std::int64_t chat_id_, std::int64_t message_id_);

  static const std::int32_t ID = -739088005;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class optimizeStorage final : public Function {
 public:
  std::int64_t size_;
  std::int32_t ttl_;
  std::int32_t count_;
  std::int32_t immunity_delay_;
  std::vector<object_ptr<FileType>> file_types_;
  std::vector<std::int64_t> chat_ids_;
  std::vector<std::int64_t> exclude_chat_ids_;
  std::int32_t chat_limit_;

  optimizeStorage();

  optimizeStorage(std::int64_t size_, std::int32_t ttl_, std::int32_t count_, std::int32_t immunity_delay_, std::vector<object_ptr<FileType>> &&file_types_, std::vector<std::int64_t> &&chat_ids_, std::vector<std::int64_t> &&exclude_chat_ids_, std::int32_t chat_limit_);

  static const std::int32_t ID = 980397489;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<storageStatistics>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class parseTextEntities final : public Function {
 public:
  std::string text_;
  object_ptr<TextParseMode> parse_mode_;

  parseTextEntities();

  parseTextEntities(std::string const &text_, object_ptr<TextParseMode> &&parse_mode_);

  static const std::int32_t ID = -1709194593;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<formattedText>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pinChatMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  bool disable_notification_;

  pinChatMessage();

  pinChatMessage(std::int64_t chat_id_, std::int64_t message_id_, bool disable_notification_);

  static const std::int32_t ID = -554712351;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class pingProxy final : public Function {
 public:
  std::int32_t proxy_id_;

  pingProxy();

  explicit pingProxy(std::int32_t proxy_id_);

  static const std::int32_t ID = -979681103;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<seconds>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class processPushNotification final : public Function {
 public:
  std::string payload_;

  processPushNotification();

  explicit processPushNotification(std::string const &payload_);

  static const std::int32_t ID = 786679952;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class readAllChatMentions final : public Function {
 public:
  std::int64_t chat_id_;

  readAllChatMentions();

  explicit readAllChatMentions(std::int64_t chat_id_);

  static const std::int32_t ID = 1357558453;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class readFilePart final : public Function {
 public:
  std::int32_t file_id_;
  std::int32_t offset_;
  std::int32_t count_;

  readFilePart();

  readFilePart(std::int32_t file_id_, std::int32_t offset_, std::int32_t count_);

  static const std::int32_t ID = -407749314;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<filePart>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class recoverAuthenticationPassword final : public Function {
 public:
  std::string recovery_code_;

  recoverAuthenticationPassword();

  explicit recoverAuthenticationPassword(std::string const &recovery_code_);

  static const std::int32_t ID = 787436412;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class recoverPassword final : public Function {
 public:
  std::string recovery_code_;

  recoverPassword();

  explicit recoverPassword(std::string const &recovery_code_);

  static const std::int32_t ID = 1660185903;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class registerDevice final : public Function {
 public:
  object_ptr<DeviceToken> device_token_;
  std::vector<std::int32_t> other_user_ids_;

  registerDevice();

  registerDevice(object_ptr<DeviceToken> &&device_token_, std::vector<std::int32_t> &&other_user_ids_);

  static const std::int32_t ID = 1734127493;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<pushReceiverId>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class registerUser final : public Function {
 public:
  std::string first_name_;
  std::string last_name_;

  registerUser();

  registerUser(std::string const &first_name_, std::string const &last_name_);

  static const std::int32_t ID = -109994467;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeBackground final : public Function {
 public:
  std::int64_t background_id_;

  removeBackground();

  explicit removeBackground(std::int64_t background_id_);

  static const std::int32_t ID = -1484545642;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeChatActionBar final : public Function {
 public:
  std::int64_t chat_id_;

  removeChatActionBar();

  explicit removeChatActionBar(std::int64_t chat_id_);

  static const std::int32_t ID = -1650968070;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeContacts final : public Function {
 public:
  std::vector<std::int32_t> user_ids_;

  removeContacts();

  explicit removeContacts(std::vector<std::int32_t> &&user_ids_);

  static const std::int32_t ID = -615510759;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeFavoriteSticker final : public Function {
 public:
  object_ptr<InputFile> sticker_;

  removeFavoriteSticker();

  explicit removeFavoriteSticker(object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = 1152945264;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeNotification final : public Function {
 public:
  std::int32_t notification_group_id_;
  std::int32_t notification_id_;

  removeNotification();

  removeNotification(std::int32_t notification_group_id_, std::int32_t notification_id_);

  static const std::int32_t ID = 862630734;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeNotificationGroup final : public Function {
 public:
  std::int32_t notification_group_id_;
  std::int32_t max_notification_id_;

  removeNotificationGroup();

  removeNotificationGroup(std::int32_t notification_group_id_, std::int32_t max_notification_id_);

  static const std::int32_t ID = 1713005454;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeProxy final : public Function {
 public:
  std::int32_t proxy_id_;

  removeProxy();

  explicit removeProxy(std::int32_t proxy_id_);

  static const std::int32_t ID = 1369219847;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeRecentHashtag final : public Function {
 public:
  std::string hashtag_;

  removeRecentHashtag();

  explicit removeRecentHashtag(std::string const &hashtag_);

  static const std::int32_t ID = -1013735260;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeRecentSticker final : public Function {
 public:
  bool is_attached_;
  object_ptr<InputFile> sticker_;

  removeRecentSticker();

  removeRecentSticker(bool is_attached_, object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = 1246577677;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeRecentlyFoundChat final : public Function {
 public:
  std::int64_t chat_id_;

  removeRecentlyFoundChat();

  explicit removeRecentlyFoundChat(std::int64_t chat_id_);

  static const std::int32_t ID = 717340444;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeSavedAnimation final : public Function {
 public:
  object_ptr<InputFile> animation_;

  removeSavedAnimation();

  explicit removeSavedAnimation(object_ptr<InputFile> &&animation_);

  static const std::int32_t ID = -495605479;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeStickerFromSet final : public Function {
 public:
  object_ptr<InputFile> sticker_;

  removeStickerFromSet();

  explicit removeStickerFromSet(object_ptr<InputFile> &&sticker_);

  static const std::int32_t ID = 1642196644;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class removeTopChat final : public Function {
 public:
  object_ptr<TopChatCategory> category_;
  std::int64_t chat_id_;

  removeTopChat();

  removeTopChat(object_ptr<TopChatCategory> &&category_, std::int64_t chat_id_);

  static const std::int32_t ID = -1907876267;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class reorderInstalledStickerSets final : public Function {
 public:
  bool is_masks_;
  std::vector<std::int64_t> sticker_set_ids_;

  reorderInstalledStickerSets();

  reorderInstalledStickerSets(bool is_masks_, std::vector<std::int64_t> &&sticker_set_ids_);

  static const std::int32_t ID = 1114537563;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class reportChat final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatReportReason> reason_;
  std::vector<std::int64_t> message_ids_;

  reportChat();

  reportChat(std::int64_t chat_id_, object_ptr<ChatReportReason> &&reason_, std::vector<std::int64_t> &&message_ids_);

  static const std::int32_t ID = -312579772;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class reportSupergroupSpam final : public Function {
 public:
  std::int32_t supergroup_id_;
  std::int32_t user_id_;
  std::vector<std::int64_t> message_ids_;

  reportSupergroupSpam();

  reportSupergroupSpam(std::int32_t supergroup_id_, std::int32_t user_id_, std::vector<std::int64_t> &&message_ids_);

  static const std::int32_t ID = -2125451498;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class requestAuthenticationPasswordRecovery final : public Function {
 public:

  requestAuthenticationPasswordRecovery();

  static const std::int32_t ID = 1393896118;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class requestPasswordRecovery final : public Function {
 public:

  requestPasswordRecovery();

  static const std::int32_t ID = -13777582;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<emailAddressAuthenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class requestQrCodeAuthentication final : public Function {
 public:
  std::vector<std::int32_t> other_user_ids_;

  requestQrCodeAuthentication();

  explicit requestQrCodeAuthentication(std::vector<std::int32_t> &&other_user_ids_);

  static const std::int32_t ID = -104224560;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendAuthenticationCode final : public Function {
 public:

  resendAuthenticationCode();

  static const std::int32_t ID = -814377191;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendChangePhoneNumberCode final : public Function {
 public:

  resendChangePhoneNumberCode();

  static const std::int32_t ID = -786772060;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendEmailAddressVerificationCode final : public Function {
 public:

  resendEmailAddressVerificationCode();

  static const std::int32_t ID = -1872416732;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<emailAddressAuthenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::vector<std::int64_t> message_ids_;

  resendMessages();

  resendMessages(std::int64_t chat_id_, std::vector<std::int64_t> &&message_ids_);

  static const std::int32_t ID = -940655817;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendPhoneNumberConfirmationCode final : public Function {
 public:

  resendPhoneNumberConfirmationCode();

  static const std::int32_t ID = 2069068522;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendPhoneNumberVerificationCode final : public Function {
 public:

  resendPhoneNumberVerificationCode();

  static const std::int32_t ID = 1367629820;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resendRecoveryEmailAddressCode final : public Function {
 public:

  resendRecoveryEmailAddressCode();

  static const std::int32_t ID = 433483548;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resetAllNotificationSettings final : public Function {
 public:

  resetAllNotificationSettings();

  static const std::int32_t ID = -174020359;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resetBackgrounds final : public Function {
 public:

  resetBackgrounds();

  static const std::int32_t ID = 204852088;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class resetNetworkStatistics final : public Function {
 public:

  resetNetworkStatistics();

  static const std::int32_t ID = 1646452102;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class saveApplicationLogEvent final : public Function {
 public:
  std::string type_;
  std::int64_t chat_id_;
  object_ptr<JsonValue> data_;

  saveApplicationLogEvent();

  saveApplicationLogEvent(std::string const &type_, std::int64_t chat_id_, object_ptr<JsonValue> &&data_);

  static const std::int32_t ID = -811154930;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchBackground final : public Function {
 public:
  std::string name_;

  searchBackground();

  explicit searchBackground(std::string const &name_);

  static const std::int32_t ID = -2130996959;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<background>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchCallMessages final : public Function {
 public:
  std::int64_t from_message_id_;
  std::int32_t limit_;
  bool only_missed_;

  searchCallMessages();

  searchCallMessages(std::int64_t from_message_id_, std::int32_t limit_, bool only_missed_);

  static const std::int32_t ID = -1077230820;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChatMembers final : public Function {
 public:
  std::int64_t chat_id_;
  std::string query_;
  std::int32_t limit_;
  object_ptr<ChatMembersFilter> filter_;

  searchChatMembers();

  searchChatMembers(std::int64_t chat_id_, std::string const &query_, std::int32_t limit_, object_ptr<ChatMembersFilter> &&filter_);

  static const std::int32_t ID = -445823291;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatMembers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChatMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::string query_;
  std::int32_t sender_user_id_;
  std::int64_t from_message_id_;
  std::int32_t offset_;
  std::int32_t limit_;
  object_ptr<SearchMessagesFilter> filter_;

  searchChatMessages();

  searchChatMessages(std::int64_t chat_id_, std::string const &query_, std::int32_t sender_user_id_, std::int64_t from_message_id_, std::int32_t offset_, std::int32_t limit_, object_ptr<SearchMessagesFilter> &&filter_);

  static const std::int32_t ID = -1528846671;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChatRecentLocationMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t limit_;

  searchChatRecentLocationMessages();

  searchChatRecentLocationMessages(std::int64_t chat_id_, std::int32_t limit_);

  static const std::int32_t ID = 950238950;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChats final : public Function {
 public:
  std::string query_;
  std::int32_t limit_;

  searchChats();

  searchChats(std::string const &query_, std::int32_t limit_);

  static const std::int32_t ID = -1879787060;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChatsNearby final : public Function {
 public:
  object_ptr<location> location_;

  searchChatsNearby();

  explicit searchChatsNearby(object_ptr<location> &&location_);

  static const std::int32_t ID = -196753377;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chatsNearby>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchChatsOnServer final : public Function {
 public:
  std::string query_;
  std::int32_t limit_;

  searchChatsOnServer();

  searchChatsOnServer(std::string const &query_, std::int32_t limit_);

  static const std::int32_t ID = -1158402188;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchContacts final : public Function {
 public:
  std::string query_;
  std::int32_t limit_;

  searchContacts();

  searchContacts(std::string const &query_, std::int32_t limit_);

  static const std::int32_t ID = -1794690715;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<users>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchEmojis final : public Function {
 public:
  std::string text_;
  bool exact_match_;
  std::string input_language_code_;

  searchEmojis();

  searchEmojis(std::string const &text_, bool exact_match_, std::string const &input_language_code_);

  static const std::int32_t ID = 453292808;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<emojis>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchHashtags final : public Function {
 public:
  std::string prefix_;
  std::int32_t limit_;

  searchHashtags();

  searchHashtags(std::string const &prefix_, std::int32_t limit_);

  static const std::int32_t ID = 1043637617;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<hashtags>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchInstalledStickerSets final : public Function {
 public:
  bool is_masks_;
  std::string query_;
  std::int32_t limit_;

  searchInstalledStickerSets();

  searchInstalledStickerSets(bool is_masks_, std::string const &query_, std::int32_t limit_);

  static const std::int32_t ID = 681171344;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchMessages final : public Function {
 public:
  object_ptr<ChatList> chat_list_;
  std::string query_;
  std::int32_t offset_date_;
  std::int64_t offset_chat_id_;
  std::int64_t offset_message_id_;
  std::int32_t limit_;

  searchMessages();

  searchMessages(object_ptr<ChatList> &&chat_list_, std::string const &query_, std::int32_t offset_date_, std::int64_t offset_chat_id_, std::int64_t offset_message_id_, std::int32_t limit_);

  static const std::int32_t ID = -455843835;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchPublicChat final : public Function {
 public:
  std::string username_;

  searchPublicChat();

  explicit searchPublicChat(std::string const &username_);

  static const std::int32_t ID = 857135533;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchPublicChats final : public Function {
 public:
  std::string query_;

  searchPublicChats();

  explicit searchPublicChats(std::string const &query_);

  static const std::int32_t ID = 970385337;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chats>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchSecretMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::string query_;
  std::int64_t from_search_id_;
  std::int32_t limit_;
  object_ptr<SearchMessagesFilter> filter_;

  searchSecretMessages();

  searchSecretMessages(std::int64_t chat_id_, std::string const &query_, std::int64_t from_search_id_, std::int32_t limit_, object_ptr<SearchMessagesFilter> &&filter_);

  static const std::int32_t ID = -1670627915;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<foundMessages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchStickerSet final : public Function {
 public:
  std::string name_;

  searchStickerSet();

  explicit searchStickerSet(std::string const &name_);

  static const std::int32_t ID = 1157930222;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSet>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchStickerSets final : public Function {
 public:
  std::string query_;

  searchStickerSets();

  explicit searchStickerSets(std::string const &query_);

  static const std::int32_t ID = -1082314629;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickerSets>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class searchStickers final : public Function {
 public:
  std::string emoji_;
  std::int32_t limit_;

  searchStickers();

  searchStickers(std::string const &emoji_, std::int32_t limit_);

  static const std::int32_t ID = 1555771203;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<stickers>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendBotStartMessage final : public Function {
 public:
  std::int32_t bot_user_id_;
  std::int64_t chat_id_;
  std::string parameter_;

  sendBotStartMessage();

  sendBotStartMessage(std::int32_t bot_user_id_, std::int64_t chat_id_, std::string const &parameter_);

  static const std::int32_t ID = 1112181339;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendCallDebugInformation final : public Function {
 public:
  std::int32_t call_id_;
  std::string debug_information_;

  sendCallDebugInformation();

  sendCallDebugInformation(std::int32_t call_id_, std::string const &debug_information_);

  static const std::int32_t ID = 2019243839;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendCallRating final : public Function {
 public:
  std::int32_t call_id_;
  std::int32_t rating_;
  std::string comment_;
  std::vector<object_ptr<CallProblem>> problems_;

  sendCallRating();

  sendCallRating(std::int32_t call_id_, std::int32_t rating_, std::string const &comment_, std::vector<object_ptr<CallProblem>> &&problems_);

  static const std::int32_t ID = -1402719502;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendChatAction final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatAction> action_;

  sendChatAction();

  sendChatAction(std::int64_t chat_id_, object_ptr<ChatAction> &&action_);

  static const std::int32_t ID = -841357536;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendChatScreenshotTakenNotification final : public Function {
 public:
  std::int64_t chat_id_;

  sendChatScreenshotTakenNotification();

  explicit sendChatScreenshotTakenNotification(std::int64_t chat_id_);

  static const std::int32_t ID = 448399457;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendChatSetTtlMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t ttl_;

  sendChatSetTtlMessage();

  sendChatSetTtlMessage(std::int64_t chat_id_, std::int32_t ttl_);

  static const std::int32_t ID = 1432535564;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendCustomRequest final : public Function {
 public:
  std::string method_;
  std::string parameters_;

  sendCustomRequest();

  sendCustomRequest(std::string const &method_, std::string const &parameters_);

  static const std::int32_t ID = 285045153;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<customRequestResult>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendEmailAddressVerificationCode final : public Function {
 public:
  std::string email_address_;

  sendEmailAddressVerificationCode();

  explicit sendEmailAddressVerificationCode(std::string const &email_address_);

  static const std::int32_t ID = -221621379;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<emailAddressAuthenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendInlineQueryResultMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t reply_to_message_id_;
  object_ptr<sendMessageOptions> options_;
  std::int64_t query_id_;
  std::string result_id_;
  bool hide_via_bot_;

  sendInlineQueryResultMessage();

  sendInlineQueryResultMessage(std::int64_t chat_id_, std::int64_t reply_to_message_id_, object_ptr<sendMessageOptions> &&options_, std::int64_t query_id_, std::string const &result_id_, bool hide_via_bot_);

  static const std::int32_t ID = 729880339;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendMessage final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t reply_to_message_id_;
  object_ptr<sendMessageOptions> options_;
  object_ptr<ReplyMarkup> reply_markup_;
  object_ptr<InputMessageContent> input_message_content_;

  sendMessage();

  sendMessage(std::int64_t chat_id_, std::int64_t reply_to_message_id_, object_ptr<sendMessageOptions> &&options_, object_ptr<ReplyMarkup> &&reply_markup_, object_ptr<InputMessageContent> &&input_message_content_);

  static const std::int32_t ID = -1314396596;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendMessageAlbum final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t reply_to_message_id_;
  object_ptr<sendMessageOptions> options_;
  std::vector<object_ptr<InputMessageContent>> input_message_contents_;

  sendMessageAlbum();

  sendMessageAlbum(std::int64_t chat_id_, std::int64_t reply_to_message_id_, object_ptr<sendMessageOptions> &&options_, std::vector<object_ptr<InputMessageContent>> &&input_message_contents_);

  static const std::int32_t ID = -818794592;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<messages>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendPassportAuthorizationForm final : public Function {
 public:
  std::int32_t autorization_form_id_;
  std::vector<object_ptr<PassportElementType>> types_;

  sendPassportAuthorizationForm();

  sendPassportAuthorizationForm(std::int32_t autorization_form_id_, std::vector<object_ptr<PassportElementType>> &&types_);

  static const std::int32_t ID = -602402218;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendPaymentForm final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::string order_info_id_;
  std::string shipping_option_id_;
  object_ptr<InputCredentials> credentials_;

  sendPaymentForm();

  sendPaymentForm(std::int64_t chat_id_, std::int64_t message_id_, std::string const &order_info_id_, std::string const &shipping_option_id_, object_ptr<InputCredentials> &&credentials_);

  static const std::int32_t ID = 591581572;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<paymentResult>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendPhoneNumberConfirmationCode final : public Function {
 public:
  std::string hash_;
  std::string phone_number_;
  object_ptr<phoneNumberAuthenticationSettings> settings_;

  sendPhoneNumberConfirmationCode();

  sendPhoneNumberConfirmationCode(std::string const &hash_, std::string const &phone_number_, object_ptr<phoneNumberAuthenticationSettings> &&settings_);

  static const std::int32_t ID = -1901171495;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sendPhoneNumberVerificationCode final : public Function {
 public:
  std::string phone_number_;
  object_ptr<phoneNumberAuthenticationSettings> settings_;

  sendPhoneNumberVerificationCode();

  sendPhoneNumberVerificationCode(std::string const &phone_number_, object_ptr<phoneNumberAuthenticationSettings> &&settings_);

  static const std::int32_t ID = 2081689035;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<authenticationCodeInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setAccountTtl final : public Function {
 public:
  object_ptr<accountTtl> ttl_;

  setAccountTtl();

  explicit setAccountTtl(object_ptr<accountTtl> &&ttl_);

  static const std::int32_t ID = 701389032;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setAlarm final : public Function {
 public:
  double seconds_;

  setAlarm();

  explicit setAlarm(double seconds_);

  static const std::int32_t ID = -873497067;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setAuthenticationPhoneNumber final : public Function {
 public:
  std::string phone_number_;
  object_ptr<phoneNumberAuthenticationSettings> settings_;

  setAuthenticationPhoneNumber();

  setAuthenticationPhoneNumber(std::string const &phone_number_, object_ptr<phoneNumberAuthenticationSettings> &&settings_);

  static const std::int32_t ID = 868276259;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setAutoDownloadSettings final : public Function {
 public:
  object_ptr<autoDownloadSettings> settings_;
  object_ptr<NetworkType> type_;

  setAutoDownloadSettings();

  setAutoDownloadSettings(object_ptr<autoDownloadSettings> &&settings_, object_ptr<NetworkType> &&type_);

  static const std::int32_t ID = -353671948;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setBackground final : public Function {
 public:
  object_ptr<InputBackground> background_;
  object_ptr<BackgroundType> type_;
  bool for_dark_theme_;

  setBackground();

  setBackground(object_ptr<InputBackground> &&background_, object_ptr<BackgroundType> &&type_, bool for_dark_theme_);

  static const std::int32_t ID = -1035439225;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<background>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setBio final : public Function {
 public:
  std::string bio_;

  setBio();

  explicit setBio(std::string const &bio_);

  static const std::int32_t ID = -1619582124;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setBotUpdatesStatus final : public Function {
 public:
  std::int32_t pending_update_count_;
  std::string error_message_;

  setBotUpdatesStatus();

  setBotUpdatesStatus(std::int32_t pending_update_count_, std::string const &error_message_);

  static const std::int32_t ID = -1154926191;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatChatList final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<ChatList> chat_list_;

  setChatChatList();

  setChatChatList(std::int64_t chat_id_, object_ptr<ChatList> &&chat_list_);

  static const std::int32_t ID = -1396517321;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatClientData final : public Function {
 public:
  std::int64_t chat_id_;
  std::string client_data_;

  setChatClientData();

  setChatClientData(std::int64_t chat_id_, std::string const &client_data_);

  static const std::int32_t ID = -827119811;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatDescription final : public Function {
 public:
  std::int64_t chat_id_;
  std::string description_;

  setChatDescription();

  setChatDescription(std::int64_t chat_id_, std::string const &description_);

  static const std::int32_t ID = 1957213277;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatDiscussionGroup final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t discussion_chat_id_;

  setChatDiscussionGroup();

  setChatDiscussionGroup(std::int64_t chat_id_, std::int64_t discussion_chat_id_);

  static const std::int32_t ID = -918801736;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatDraftMessage final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<draftMessage> draft_message_;

  setChatDraftMessage();

  setChatDraftMessage(std::int64_t chat_id_, object_ptr<draftMessage> &&draft_message_);

  static const std::int32_t ID = -588175579;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatLocation final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<chatLocation> location_;

  setChatLocation();

  setChatLocation(std::int64_t chat_id_, object_ptr<chatLocation> &&location_);

  static const std::int32_t ID = -767091286;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatMemberStatus final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;
  object_ptr<ChatMemberStatus> status_;

  setChatMemberStatus();

  setChatMemberStatus(std::int64_t chat_id_, std::int32_t user_id_, object_ptr<ChatMemberStatus> &&status_);

  static const std::int32_t ID = -1754439241;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatNotificationSettings final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<chatNotificationSettings> notification_settings_;

  setChatNotificationSettings();

  setChatNotificationSettings(std::int64_t chat_id_, object_ptr<chatNotificationSettings> &&notification_settings_);

  static const std::int32_t ID = 777199614;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatPermissions final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<chatPermissions> permissions_;

  setChatPermissions();

  setChatPermissions(std::int64_t chat_id_, object_ptr<chatPermissions> &&permissions_);

  static const std::int32_t ID = 2138507006;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatPhoto final : public Function {
 public:
  std::int64_t chat_id_;
  object_ptr<InputFile> photo_;

  setChatPhoto();

  setChatPhoto(std::int64_t chat_id_, object_ptr<InputFile> &&photo_);

  static const std::int32_t ID = 132244217;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatSlowModeDelay final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t slow_mode_delay_;

  setChatSlowModeDelay();

  setChatSlowModeDelay(std::int64_t chat_id_, std::int32_t slow_mode_delay_);

  static const std::int32_t ID = -540350914;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setChatTitle final : public Function {
 public:
  std::int64_t chat_id_;
  std::string title_;

  setChatTitle();

  setChatTitle(std::int64_t chat_id_, std::string const &title_);

  static const std::int32_t ID = 164282047;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setCustomLanguagePack final : public Function {
 public:
  object_ptr<languagePackInfo> info_;
  std::vector<object_ptr<languagePackString>> strings_;

  setCustomLanguagePack();

  setCustomLanguagePack(object_ptr<languagePackInfo> &&info_, std::vector<object_ptr<languagePackString>> &&strings_);

  static const std::int32_t ID = -296742819;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setCustomLanguagePackString final : public Function {
 public:
  std::string language_pack_id_;
  object_ptr<languagePackString> new_string_;

  setCustomLanguagePackString();

  setCustomLanguagePackString(std::string const &language_pack_id_, object_ptr<languagePackString> &&new_string_);

  static const std::int32_t ID = 1316365592;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setDatabaseEncryptionKey final : public Function {
 public:
  std::string new_encryption_key_;

  setDatabaseEncryptionKey();

  explicit setDatabaseEncryptionKey(std::string const &new_encryption_key_);

  static const std::int32_t ID = -1204599371;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setFileGenerationProgress final : public Function {
 public:
  std::int64_t generation_id_;
  std::int32_t expected_size_;
  std::int32_t local_prefix_size_;

  setFileGenerationProgress();

  setFileGenerationProgress(std::int64_t generation_id_, std::int32_t expected_size_, std::int32_t local_prefix_size_);

  static const std::int32_t ID = -540459953;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setGameScore final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  bool edit_message_;
  std::int32_t user_id_;
  std::int32_t score_;
  bool force_;

  setGameScore();

  setGameScore(std::int64_t chat_id_, std::int64_t message_id_, bool edit_message_, std::int32_t user_id_, std::int32_t score_, bool force_);

  static const std::int32_t ID = -1768307069;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<message>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setInlineGameScore final : public Function {
 public:
  std::string inline_message_id_;
  bool edit_message_;
  std::int32_t user_id_;
  std::int32_t score_;
  bool force_;

  setInlineGameScore();

  setInlineGameScore(std::string const &inline_message_id_, bool edit_message_, std::int32_t user_id_, std::int32_t score_, bool force_);

  static const std::int32_t ID = 758435487;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setLogStream final : public Function {
 public:
  object_ptr<LogStream> log_stream_;

  setLogStream();

  explicit setLogStream(object_ptr<LogStream> &&log_stream_);

  static const std::int32_t ID = -1364199535;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setLogTagVerbosityLevel final : public Function {
 public:
  std::string tag_;
  std::int32_t new_verbosity_level_;

  setLogTagVerbosityLevel();

  setLogTagVerbosityLevel(std::string const &tag_, std::int32_t new_verbosity_level_);

  static const std::int32_t ID = -2095589738;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setLogVerbosityLevel final : public Function {
 public:
  std::int32_t new_verbosity_level_;

  setLogVerbosityLevel();

  explicit setLogVerbosityLevel(std::int32_t new_verbosity_level_);

  static const std::int32_t ID = -303429678;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setName final : public Function {
 public:
  std::string first_name_;
  std::string last_name_;

  setName();

  setName(std::string const &first_name_, std::string const &last_name_);

  static const std::int32_t ID = 1711693584;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setNetworkType final : public Function {
 public:
  object_ptr<NetworkType> type_;

  setNetworkType();

  explicit setNetworkType(object_ptr<NetworkType> &&type_);

  static const std::int32_t ID = -701635234;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setOption final : public Function {
 public:
  std::string name_;
  object_ptr<OptionValue> value_;

  setOption();

  setOption(std::string const &name_, object_ptr<OptionValue> &&value_);

  static const std::int32_t ID = 2114670322;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setPassportElement final : public Function {
 public:
  object_ptr<InputPassportElement> element_;
  std::string password_;

  setPassportElement();

  setPassportElement(object_ptr<InputPassportElement> &&element_, std::string const &password_);

  static const std::int32_t ID = 2068173212;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<PassportElement>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setPassportElementErrors final : public Function {
 public:
  std::int32_t user_id_;
  std::vector<object_ptr<inputPassportElementError>> errors_;

  setPassportElementErrors();

  setPassportElementErrors(std::int32_t user_id_, std::vector<object_ptr<inputPassportElementError>> &&errors_);

  static const std::int32_t ID = 1455869875;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setPassword final : public Function {
 public:
  std::string old_password_;
  std::string new_password_;
  std::string new_hint_;
  bool set_recovery_email_address_;
  std::string new_recovery_email_address_;

  setPassword();

  setPassword(std::string const &old_password_, std::string const &new_password_, std::string const &new_hint_, bool set_recovery_email_address_, std::string const &new_recovery_email_address_);

  static const std::int32_t ID = -1193589027;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setPinnedChats final : public Function {
 public:
  object_ptr<ChatList> chat_list_;
  std::vector<std::int64_t> chat_ids_;

  setPinnedChats();

  setPinnedChats(object_ptr<ChatList> &&chat_list_, std::vector<std::int64_t> &&chat_ids_);

  static const std::int32_t ID = -695640000;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setPollAnswer final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  std::vector<std::int32_t> option_ids_;

  setPollAnswer();

  setPollAnswer(std::int64_t chat_id_, std::int64_t message_id_, std::vector<std::int32_t> &&option_ids_);

  static const std::int32_t ID = -1399388792;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setProfilePhoto final : public Function {
 public:
  object_ptr<InputFile> photo_;

  setProfilePhoto();

  explicit setProfilePhoto(object_ptr<InputFile> &&photo_);

  static const std::int32_t ID = 1594734550;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setRecoveryEmailAddress final : public Function {
 public:
  std::string password_;
  std::string new_recovery_email_address_;

  setRecoveryEmailAddress();

  setRecoveryEmailAddress(std::string const &password_, std::string const &new_recovery_email_address_);

  static const std::int32_t ID = -1981836385;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<passwordState>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setScopeNotificationSettings final : public Function {
 public:
  object_ptr<NotificationSettingsScope> scope_;
  object_ptr<scopeNotificationSettings> notification_settings_;

  setScopeNotificationSettings();

  setScopeNotificationSettings(object_ptr<NotificationSettingsScope> &&scope_, object_ptr<scopeNotificationSettings> &&notification_settings_);

  static const std::int32_t ID = -2049984966;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setStickerPositionInSet final : public Function {
 public:
  object_ptr<InputFile> sticker_;
  std::int32_t position_;

  setStickerPositionInSet();

  setStickerPositionInSet(object_ptr<InputFile> &&sticker_, std::int32_t position_);

  static const std::int32_t ID = 2075281185;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setSupergroupStickerSet final : public Function {
 public:
  std::int32_t supergroup_id_;
  std::int64_t sticker_set_id_;

  setSupergroupStickerSet();

  setSupergroupStickerSet(std::int32_t supergroup_id_, std::int64_t sticker_set_id_);

  static const std::int32_t ID = -295782298;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setSupergroupUsername final : public Function {
 public:
  std::int32_t supergroup_id_;
  std::string username_;

  setSupergroupUsername();

  setSupergroupUsername(std::int32_t supergroup_id_, std::string const &username_);

  static const std::int32_t ID = -1428333122;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setTdlibParameters final : public Function {
 public:
  object_ptr<tdlibParameters> parameters_;

  setTdlibParameters();

  explicit setTdlibParameters(object_ptr<tdlibParameters> &&parameters_);

  static const std::int32_t ID = -1912557997;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setUserPrivacySettingRules final : public Function {
 public:
  object_ptr<UserPrivacySetting> setting_;
  object_ptr<userPrivacySettingRules> rules_;

  setUserPrivacySettingRules();

  setUserPrivacySettingRules(object_ptr<UserPrivacySetting> &&setting_, object_ptr<userPrivacySettingRules> &&rules_);

  static const std::int32_t ID = -473812741;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class setUsername final : public Function {
 public:
  std::string username_;

  setUsername();

  explicit setUsername(std::string const &username_);

  static const std::int32_t ID = 439901214;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class sharePhoneNumber final : public Function {
 public:
  std::int32_t user_id_;

  sharePhoneNumber();

  explicit sharePhoneNumber(std::int32_t user_id_);

  static const std::int32_t ID = -370669878;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class stopPoll final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<ReplyMarkup> reply_markup_;

  stopPoll();

  stopPoll(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<ReplyMarkup> &&reply_markup_);

  static const std::int32_t ID = 1659374253;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class synchronizeLanguagePack final : public Function {
 public:
  std::string language_pack_id_;

  synchronizeLanguagePack();

  explicit synchronizeLanguagePack(std::string const &language_pack_id_);

  static const std::int32_t ID = -2065307858;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class terminateAllOtherSessions final : public Function {
 public:

  terminateAllOtherSessions();

  static const std::int32_t ID = 1874485523;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class terminateSession final : public Function {
 public:
  std::int64_t session_id_;

  terminateSession();

  explicit terminateSession(std::int64_t session_id_);

  static const std::int32_t ID = -407385812;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallBytes final : public Function {
 public:
  std::string x_;

  testCallBytes();

  explicit testCallBytes(std::string const &x_);

  static const std::int32_t ID = -736011607;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testBytes>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallEmpty final : public Function {
 public:

  testCallEmpty();

  static const std::int32_t ID = -627291626;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallString final : public Function {
 public:
  std::string x_;

  testCallString();

  explicit testCallString(std::string const &x_);

  static const std::int32_t ID = -1732818385;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testString>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallVectorInt final : public Function {
 public:
  std::vector<std::int32_t> x_;

  testCallVectorInt();

  explicit testCallVectorInt(std::vector<std::int32_t> &&x_);

  static const std::int32_t ID = -2137277793;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testVectorInt>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallVectorIntObject final : public Function {
 public:
  std::vector<object_ptr<testInt>> x_;

  testCallVectorIntObject();

  explicit testCallVectorIntObject(std::vector<object_ptr<testInt>> &&x_);

  static const std::int32_t ID = 1825428218;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testVectorIntObject>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallVectorString final : public Function {
 public:
  std::vector<std::string> x_;

  testCallVectorString();

  explicit testCallVectorString(std::vector<std::string> &&x_);

  static const std::int32_t ID = -408600900;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testVectorString>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testCallVectorStringObject final : public Function {
 public:
  std::vector<object_ptr<testString>> x_;

  testCallVectorStringObject();

  explicit testCallVectorStringObject(std::vector<object_ptr<testString>> &&x_);

  static const std::int32_t ID = 1527666429;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testVectorStringObject>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testGetDifference final : public Function {
 public:

  testGetDifference();

  static const std::int32_t ID = 1747084069;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testNetwork final : public Function {
 public:

  testNetwork();

  static const std::int32_t ID = -1343998901;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testProxy final : public Function {
 public:
  std::string server_;
  std::int32_t port_;
  object_ptr<ProxyType> type_;
  std::int32_t dc_id_;
  double timeout_;

  testProxy();

  testProxy(std::string const &server_, std::int32_t port_, object_ptr<ProxyType> &&type_, std::int32_t dc_id_, double timeout_);

  static const std::int32_t ID = -1197366626;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testReturnError final : public Function {
 public:
  object_ptr<error> error_;

  testReturnError();

  explicit testReturnError(object_ptr<error> &&error_);

  static const std::int32_t ID = 455179506;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<error>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testSquareInt final : public Function {
 public:
  std::int32_t x_;

  testSquareInt();

  explicit testSquareInt(std::int32_t x_);

  static const std::int32_t ID = -60135024;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<testInt>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class testUseUpdate final : public Function {
 public:

  testUseUpdate();

  static const std::int32_t ID = 717094686;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<Update>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class toggleChatDefaultDisableNotification final : public Function {
 public:
  std::int64_t chat_id_;
  bool default_disable_notification_;

  toggleChatDefaultDisableNotification();

  toggleChatDefaultDisableNotification(std::int64_t chat_id_, bool default_disable_notification_);

  static const std::int32_t ID = 314794002;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class toggleChatIsMarkedAsUnread final : public Function {
 public:
  std::int64_t chat_id_;
  bool is_marked_as_unread_;

  toggleChatIsMarkedAsUnread();

  toggleChatIsMarkedAsUnread(std::int64_t chat_id_, bool is_marked_as_unread_);

  static const std::int32_t ID = -986129697;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class toggleChatIsPinned final : public Function {
 public:
  std::int64_t chat_id_;
  bool is_pinned_;

  toggleChatIsPinned();

  toggleChatIsPinned(std::int64_t chat_id_, bool is_pinned_);

  static const std::int32_t ID = -1166802621;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class toggleSupergroupIsAllHistoryAvailable final : public Function {
 public:
  std::int32_t supergroup_id_;
  bool is_all_history_available_;

  toggleSupergroupIsAllHistoryAvailable();

  toggleSupergroupIsAllHistoryAvailable(std::int32_t supergroup_id_, bool is_all_history_available_);

  static const std::int32_t ID = 1701526555;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class toggleSupergroupSignMessages final : public Function {
 public:
  std::int32_t supergroup_id_;
  bool sign_messages_;

  toggleSupergroupSignMessages();

  toggleSupergroupSignMessages(std::int32_t supergroup_id_, bool sign_messages_);

  static const std::int32_t ID = -558196581;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class transferChatOwnership final : public Function {
 public:
  std::int64_t chat_id_;
  std::int32_t user_id_;
  std::string password_;

  transferChatOwnership();

  transferChatOwnership(std::int64_t chat_id_, std::int32_t user_id_, std::string const &password_);

  static const std::int32_t ID = -1925047127;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class unblockUser final : public Function {
 public:
  std::int32_t user_id_;

  unblockUser();

  explicit unblockUser(std::int32_t user_id_);

  static const std::int32_t ID = -307286367;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class unpinChatMessage final : public Function {
 public:
  std::int64_t chat_id_;

  unpinChatMessage();

  explicit unpinChatMessage(std::int64_t chat_id_);

  static const std::int32_t ID = 277557690;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class upgradeBasicGroupChatToSupergroupChat final : public Function {
 public:
  std::int64_t chat_id_;

  upgradeBasicGroupChatToSupergroupChat();

  explicit upgradeBasicGroupChatToSupergroupChat(std::int64_t chat_id_);

  static const std::int32_t ID = 300488122;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<chat>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class uploadFile final : public Function {
 public:
  object_ptr<InputFile> file_;
  object_ptr<FileType> file_type_;
  std::int32_t priority_;

  uploadFile();

  uploadFile(object_ptr<InputFile> &&file_, object_ptr<FileType> &&file_type_, std::int32_t priority_);

  static const std::int32_t ID = -745597786;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class uploadStickerFile final : public Function {
 public:
  std::int32_t user_id_;
  object_ptr<InputFile> png_sticker_;

  uploadStickerFile();

  uploadStickerFile(std::int32_t user_id_, object_ptr<InputFile> &&png_sticker_);

  static const std::int32_t ID = 1134087551;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<file>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class validateOrderInfo final : public Function {
 public:
  std::int64_t chat_id_;
  std::int64_t message_id_;
  object_ptr<orderInfo> order_info_;
  bool allow_save_;

  validateOrderInfo();

  validateOrderInfo(std::int64_t chat_id_, std::int64_t message_id_, object_ptr<orderInfo> &&order_info_, bool allow_save_);

  static const std::int32_t ID = 9480644;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<validatedOrderInfo>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class viewMessages final : public Function {
 public:
  std::int64_t chat_id_;
  std::vector<std::int64_t> message_ids_;
  bool force_read_;

  viewMessages();

  viewMessages(std::int64_t chat_id_, std::vector<std::int64_t> &&message_ids_, bool force_read_);

  static const std::int32_t ID = -1925784915;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class viewTrendingStickerSets final : public Function {
 public:
  std::vector<std::int64_t> sticker_set_ids_;

  viewTrendingStickerSets();

  explicit viewTrendingStickerSets(std::vector<std::int64_t> &&sticker_set_ids_);

  static const std::int32_t ID = -952416520;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

class writeGeneratedFilePart final : public Function {
 public:
  std::int64_t generation_id_;
  std::int32_t offset_;
  std::string data_;

  writeGeneratedFilePart();

  writeGeneratedFilePart(std::int64_t generation_id_, std::int32_t offset_, std::string const &data_);

  static const std::int32_t ID = -2062358189;
  std::int32_t get_id() const final {
    return ID;
  }

  using ReturnType = object_ptr<ok>;

  void store(TlStorerToString &s, const char *field_name) const final;
};

}  // namespace td_api
}  // namespace td
