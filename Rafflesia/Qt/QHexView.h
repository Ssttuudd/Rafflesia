#ifndef Q_HEX_VIEWER_H_
#define Q_HEX_VIEWER_H_

#include <QAbstractScrollArea>
#include <QByteArray>
#include <QFile>

class QHexView: public QAbstractScrollArea

{
	public:
		class DataStorage
		{
			public:
				virtual ~DataStorage() {};
				virtual QByteArray getData(std::size_t position, std::size_t length) = 0;
				virtual std::size_t size() = 0;
		};


		class DataStorageArray: public DataStorage
		{
			public:
				DataStorageArray(const QByteArray &arr);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QByteArray    m_data;
		};

		class DataStorageFile: public DataStorage
		{
			public:
				DataStorageFile(const QString &fileName);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QFile      m_file;
		};



		QHexView(QWidget *parent = 0);
		~QHexView();

	public slots:
		void setData(DataStorage *pData);
		void clear();
		void showFromOffset(std::size_t offset);

	protected:
		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
	private:
		DataStorage          *m_pdata;
		std::size_t           m_posAddr; 
		std::size_t           m_posHex;
		std::size_t           m_posAscii;
		std::size_t           m_charWidth;
		std::size_t           m_charHeight;


		std::size_t           m_selectBegin;
		std::size_t           m_selectEnd;
		std::size_t           m_selectInit;
		std::size_t           m_cursorPos;


		QSize fullSize() const;
		void resetSelection();
		void resetSelection(int position);
		void setSelection(int position);
		void ensureVisible();
		void setCursorPos(int position);
		std::size_t cursorPos(const QPoint &position);
};

#endif
