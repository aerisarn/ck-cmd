#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QFileSystemModel>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

static inline QString permissionString(const QFileInfo &fi)
{
	const QFile::Permissions permissions = fi.permissions();
	QString result = QLatin1String("----------");
	if (fi.isSymLink())
		result[0] = QLatin1Char('l');
	else if (fi.isDir())
		result[0] = QLatin1Char('d');
	if (permissions & QFileDevice::ReadUser)
		result[1] = QLatin1Char('r');
	if (permissions & QFileDevice::WriteUser)
		result[2] = QLatin1Char('w');
	if (permissions & QFileDevice::ExeUser)
		result[3] = QLatin1Char('x');
	if (permissions & QFileDevice::ReadGroup)
		result[4] = QLatin1Char('r');
	if (permissions & QFileDevice::WriteGroup)
		result[5] = QLatin1Char('w');
	if (permissions & QFileDevice::ExeGroup)
		result[6] = QLatin1Char('x');
	if (permissions & QFileDevice::ReadOther)
		result[7] = QLatin1Char('r');
	if (permissions & QFileDevice::WriteOther)
		result[8] = QLatin1Char('w');
	if (permissions & QFileDevice::ExeOther)
		result[9] = QLatin1Char('x');
	return result;
}

static inline QString sizeString(const QFileInfo &fi)
{
	if (!fi.isFile())
		return QString();
	const qint64 size = fi.size();
	if (size > 1024 * 1024 * 10)
		return QString::number(size / (1024 * 1024)) + QLatin1Char('M');
	if (size > 1024 * 10)
		return QString::number(size / 1024) + QLatin1Char('K');
	return QString::number(size);
}

class DisplayFileSystemModel : public QFileSystemModel {
	Q_OBJECT
public:
	explicit DisplayFileSystemModel(QObject *parent = Q_NULLPTR)
		: QFileSystemModel(parent) {}

	enum Roles {
		SizeRole = Qt::UserRole + 4,
		DisplayableFilePermissionsRole = Qt::UserRole + 5,
		LastModifiedRole = Qt::UserRole + 6,
		UrlStringRole = Qt::UserRole + 7
	};
	Q_ENUM(Roles)

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
	{
		if (index.isValid() && role >= SizeRole) {
			switch (role) {
			case SizeRole:
				return QVariant(sizeString(fileInfo(index)));
			case DisplayableFilePermissionsRole:
				return QVariant(permissionString(fileInfo(index)));
			case LastModifiedRole:
				return QVariant(fileInfo(index).lastModified().toString(Qt::SystemLocaleShortDate));
			case UrlStringRole:
				return QVariant(QUrl::fromLocalFile(filePath(index)).toString());
			default:
				break;
			}
		}
		return QFileSystemModel::data(index, role);
	}

	QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE
	{
		QHash<int, QByteArray> result = QFileSystemModel::roleNames();
		result.insert(SizeRole, QByteArrayLiteral("size"));
		result.insert(DisplayableFilePermissionsRole, QByteArrayLiteral("displayableFilePermissions"));
		result.insert(LastModifiedRole, QByteArrayLiteral("lastModified"));
		return result;
	}
};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterUncreatableType<DisplayFileSystemModel>("io.qt.examples.quick.controls.filesystembrowser", 1, 0,
		"FileSystemModel", "Cannot create a FileSystemModel instance.");
	QFileSystemModel *fsm = new DisplayFileSystemModel(&engine);
	fsm->setRootPath(QDir::homePath());
	fsm->setResolveSymlinks(true);
	engine.rootContext()->setContextProperty("fileSystemModel", fsm);
	engine.rootContext()->setContextProperty("rootPathIndex", fsm->index(fsm->rootPath()));
	engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

#include "main.moc"

