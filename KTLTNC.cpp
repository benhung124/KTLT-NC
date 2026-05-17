#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <map>
#include <windows.h>
#include <commdlg.h>
#include <algorithm>
#include <limits>
#include <cctype>
#include <ctime>
#include <filesystem>

using namespace std;

void SetColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define WHITE 7

#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define LIGHT_YELLOW 14
#define BRIGHT_WHITE 15

void VeKhungTieuDe(string tieuDe, int mau)
{
    int len = tieuDe.length() + 4;
    SetColor(mau);
    cout << "\n╔";
    for (int i = 0; i < len; i++)
        cout << "═";
    cout << "╗" << endl;
    cout << "║  ";
    SetColor(BRIGHT_WHITE);
    cout << tieuDe;
    SetColor(mau);
    cout << "  ║" << endl;
    cout << "╚";
    for (int i = 0; i < len; i++)
        cout << "═";
    cout << "╝" << endl;
    SetColor(WHITE);
}
//======Tai file======
string taiFile()
{
    OPENFILENAMEW ofn;
    WCHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);

    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
    {
        // Convert wide string to narrow string
        char buffer[260];
        WideCharToMultiByte(CP_UTF8, 0, szFile, -1, buffer, sizeof(buffer), NULL, NULL);
        return string(buffer);
    }
    return "";
}

// ========================== CAU HOI ==========================
struct CauHoi
{
    int id;
    string noiDung;
    string dapAn[4];
    int dapAnDung;
};

//===================== Ham Nhap ==========================
string inputLine(const string& prompt)
{
    string s;
    while (true)
    {
        cout << prompt;
        getline(cin, s);
        if (!s.empty())
            return s;
        cout << "Loi: khong duoc de trong.\n";
    }
}

template <typename T>
T nhapSo(const string& thongBao)
{
    T value;
    while (true)
    {
        cout << thongBao;
        if (cin >> value)
        {
            cin.ignore(1000, '\n');
            return value;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Nhap sai, thu lai!\n";
    }
}

time_t parseDateTime(const string& s)
{
    tm tmVal = {};
    int year, month, day, hour, minute;
    if (sscanf(s.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute) == 5)
    {
        tmVal.tm_year = year - 1900;
        tmVal.tm_mon = month - 1;
        tmVal.tm_mday = day;
        tmVal.tm_hour = hour;
        tmVal.tm_min = minute;
        tmVal.tm_sec = 0;
        tmVal.tm_isdst = -1;
        return mktime(&tmVal);
    }
    return 0;
}

string formatDateTime(time_t t)
{
    if (t <= 0)
        return "N/A";
    tm localTime;
    localtime_s(&localTime, &t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &localTime);
    return string(buffer);
}

string hashPassword(const string& raw)
{
    return raw;
}

// ========================== DE THI ==========================
struct deThi
{
    int id;
    int khoa;
    string tenMon;
    vector<CauHoi> danhSachCauHoi;
    CauHoi ch;
    int thoiGianLamBai;
    int loaiThi;
    int phongThiId;
    time_t batDau;
    time_t ketThuc;
    bool locked;
    vector<string> dsLopDuocThi; // Lưu danh sách tên lớp được phép thi
    int soLanOnTap;              // -1 = không giới hạn, >0 = giới hạn số lần

    deThi() : id(0), khoa(0), tenMon(""), thoiGianLamBai(0),
        loaiThi(1), phongThiId(0), batDau(0), ketThuc(0),
        locked(false), soLanOnTap(-1) {
    }

    void themCauHoi()
    {
        ch.id = nhapSo<int>("ID: ");
        cout << "Noi dung: ";
        getline(cin, ch.noiDung);
        for (int i = 0; i < 4; i++)
        {
            cout << "Lua chon " << i + 1 << ": ";
            getline(cin, ch.dapAn[i]);
        }
        do
        {
            ch.dapAnDung = nhapSo<int>("Dap an dung (1-4): ");
        } while (ch.dapAnDung < 1 || ch.dapAnDung > 4);
        danhSachCauHoi.push_back(ch);
    }

    void hienThiCauHoi()
    {
        for (auto& ch : danhSachCauHoi)
        {
            cout << ch.id << ". " << ch.noiDung << endl;
        }
    }

    bool isAvailable() const
    {
        if (locked)
            return false;
        if (batDau == 0 || ketThuc == 0)
            return true;
        time_t now = time(0);
        return now >= batDau && now <= ketThuc;
    }

    string getSchedule() const
    {
        return formatDateTime(batDau) + " -> " + formatDateTime(ketThuc);
    }
};

// ========================== KET QUA ==========================
struct KetQua
{
    int idSV;
    string tenSV;
    string mon;
    double diem;
    int thoiGian;
    int maDe;
    int loaiThi;

    string getMon() const
    {
        return mon;
    }
};

// ========================== CLASS INFO ==========================
struct ClassInfo
{
    int id;
    string className;
    int coVanHocTapId;             // Thay cho teacherId cũ (1 Cố vấn học tập)
    vector<int> danhSachGiangVien; // Danh sách nhiều giáo viên giảng dạy
    int studentCount;
    int teacherId;
};

struct PhongThi
{
    int id;
    string tenPhong;
    int sucChua;
    bool locked;

    PhongThi() : id(0), tenPhong(""), sucChua(0), locked(false) {}
};

// ========================== PERSON ==========================
class Person
{
protected:
    int id;
    string username;
    string password;
    string fullName;

public:
    Person() : id(0), username(""), password(""), fullName("") {}

    Person(int id, const string& username, const string& password, const string& fullName)
        : id(id), username(username), password(password), fullName(fullName)
    {
    }

    virtual ~Person() {}
    virtual string getRole() const = 0;

    virtual bool login(const string& u, const string& p) const
    {
        return u == username && (password == p || password == hashPassword(p));
    }

    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getFullName() const { return fullName; }
    void setId(int newId) { id = newId; }
    void setUsername(const string& newUsername) { username = newUsername; }
    void setPassword(const string& newPassword) { password = newPassword; }
    void setFullName(const string& newFullName) { fullName = newFullName; }
};

// ========================== ADMIN ==========================
class Admin : public Person
{
public:
    Admin() : Person(1, "admin", "admin", "System Admin") {}

    Admin(int id, string u, string p, string ten) : Person(id, u, p, ten) {}

    string getRole() const override
    {
        return "Admin";
    }

    void hienThiThongTin() const
    {
        cout << "[ADMIN] " << fullName << endl;
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName;
    }
};

// ========================== QUAN LY THI ==========================
class QuanLyThi
{
private:
    string tenMon;
    deThi de;
    vector<KetQua> danhSachKetQua;
    vector<deThi> maDe;

public:
    void saveDeThi() const;
    void loadDeThi();
    vector<deThi>& getMaDe() { return maDe; }
    QuanLyThi()
    {
        maDe = {};
        tenMon = "Chua dat ten";
    }

    void thietLapDeThi(int teacherId, string monGiangVien, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms)
    {
        // 1. Loại bài thi TRƯỚC TIÊN
        do
        {
            de.loaiThi = nhapSo<int>("Loai bai thi (1. Chinh thuc | 2. On tap): ");
        } while (de.loaiThi != 1 && de.loaiThi != 2);

        // 2. Tên môn
        de.tenMon = monGiangVien;

        // 3. Mã đề — kiểm tra trùng
        do
        {
            de.id = nhapSo<int>("Ma de: ");
            bool trung = false;
            for (const auto& d : maDe)
            {
                if (d.id == de.id)
                {
                    trung = true;
                    break;
                }
            }
            if (trung)
            {
                SetColor(RED);
                cout << "Ma de da ton tai! Nhap lai.\n";
                SetColor(WHITE);
            }
            else
                break;
        } while (true);

        // 4. Thời gian làm bài
        de.thoiGianLamBai = nhapSo<int>("Thoi gian lam bai (giay): ");

        // 5. Phòng thi — CHỈ yêu cầu nếu Chính thức
        if (de.loaiThi == 1)
        {
            if (examRooms.empty())
            {
                SetColor(RED);
                cout << "Chua co phong thi nao! Admin can tao phong thi truoc.\n";
                SetColor(WHITE);
                return;
            }
            bool validRoom = false;
            do
            {
                de.phongThiId = nhapSo<int>("Nhap ID phong thi: ");
                for (const auto& r : examRooms)
                {
                    if (r.id == de.phongThiId)
                    {
                        validRoom = true;
                        break;
                    }
                }
                if (!validRoom)
                {
                    SetColor(RED);
                    cout << "Phong thi ID " << de.phongThiId << " khong ton tai! Nhap lai.\n";
                    SetColor(WHITE);
                }
            } while (!validRoom);
        }
        else // loaiDeThi == 2
        {
            de.phongThiId = 0;
            de.batDau = 0;
            de.ketThuc = 0;

            cout << "Gioi han so lan on tap? (0 = khong gioi han): ";
            int soLan = nhapSo<int>("");
            de.soLanOnTap = (soLan <= 0) ? -1 : soLan;

            if (de.soLanOnTap == -1)
            {
                SetColor(LIGHT_GREEN);
                cout << "De on tap khong gioi han so lan.\n";
                SetColor(WHITE);
            }
            else
            {
                SetColor(LIGHT_YELLOW);
                cout << "De on tap gioi han " << de.soLanOnTap << " lan.\n";
                SetColor(WHITE);
            }
        }

        // 6. Lịch thi — CHỈ yêu cầu nếu Chính thức
        if (de.loaiThi == 1)
        {
            do
            {
                string batDau = inputLine("Nhap ngay gio bat dau (YYYY-MM-DD HH:MM): ");
                de.batDau = parseDateTime(batDau);
                if (de.batDau == 0)
                    cout << "Dinh dang khong hop le. Thu lai!\n";
            } while (de.batDau == 0);

            do
            {
                string ketThuc = inputLine("Nhap ngay gio ket thuc (YYYY-MM-DD HH:MM): ");
                de.ketThuc = parseDateTime(ketThuc);
                if (de.ketThuc == 0 || de.ketThuc <= de.batDau)
                {
                    cout << "Khong hop le hoac ket thuc truoc bat dau. Thu lai!\n";
                    de.ketThuc = 0;
                }
            } while (de.ketThuc == 0);
        }
        else
        {
            de.batDau = 0; // Ôn tập luôn available
            de.ketThuc = 0;
        }

        // 7. Phân quyền lớp - TỰ ĐỘNG THEO CHUYÊN NGÀNH
        de.dsLopDuocThi.clear();
        SetColor(LIGHT_CYAN);
        cout << "\nDang tu dong phan quyen thi cho cac lop cung chuyen nganh...\n";
        SetColor(WHITE);

        bool hasClass = false;

        for (const auto& c : classes)
        {
            bool duocGiangDay = false;

            // Cách 1: Kiểm tra theo danh sách giảng viên (cũ)
            for (int gvId : c.danhSachGiangVien)
            {
                if (gvId == teacherId)
                {
                    duocGiangDay = true;
                    break;
                }
            }

            // Cách 2: Kiểm tra theo tên lớp + môn giảng viên (mới - ưu tiên)
            if (!duocGiangDay && !monGiangVien.empty() && !c.className.empty())
            {
                // Ví dụ: Giảng viên CNTT → cho phép CNTT01, CNTT02, CNTT03...
                if (c.className.find(monGiangVien) == 0 ||
                    c.className.rfind(monGiangVien, 0) == 0) // Bắt đầu bằng môn
                {
                    duocGiangDay = true;
                }
            }

            if (duocGiangDay)
            {
                de.dsLopDuocThi.push_back(c.className);
                SetColor(GREEN);
                cout << " + Da cap quyen cho lop: " << c.className << "\n";
                SetColor(WHITE);
                hasClass = true;
            }
        }

        if (!hasClass)
        {
            SetColor(YELLOW);
            cout << "Khong tim thay lop nao phu trach. De thi se mo cho toan truong (ALL).\n";
            SetColor(WHITE);
            de.dsLopDuocThi.clear(); // ALL
        }

        // 8. Câu hỏi
        de.locked = false;
        int m = nhapSo<int>("Nhap so cau hoi cua de: ");
        de.danhSachCauHoi.clear();
        for (int i = 0; i < m; i++)
        {
            SetColor(LIGHT_YELLOW);
            cout << "\n--- Nhap cau hoi thu " << i + 1 << " ---\n";
            SetColor(WHITE);
            de.themCauHoi();
        }

        maDe.push_back(de);
        saveDeThi();
        SetColor(LIGHT_GREEN);
        cout << "Tao de thi thanh cong!\n";
        SetColor(WHITE);
    }

    void khoaMoDeThi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can khoa/mo: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                d.locked = !d.locked;
                cout << "De thi " << d.id << " da " << (d.locked ? "duoc khoa" : "duoc mo") << ".\n";
                saveDeThi();
                return;
            }
        }
        cout << "Khong tim thay de thi!\n";
    }

    void xemLichThi()
    {
        if (maDe.empty())
        {
            cout << "Chua co de thi de hien thi lich.\n";
            return;
        }
        SetColor(CYAN);
        cout << "\n===== LICH THI =====\n";
        SetColor(WHITE);
        for (const auto& d : maDe)
        {
            cout << "Ma de: " << d.id
                << " | Mon: " << d.tenMon
                << " | Phong: " << d.phongThiId
                << " | Lich: " << d.getSchedule()
                << " | Trang thai: " << (d.locked ? "Khoa" : (d.isAvailable() ? "Dang mo" : "Chua den gio/da ket thuc"))
                << "\n";
        }
    }

    const deThi* timDeThi(int id) const
    {
        for (const auto& d : maDe)
        {
            if (d.id == id)
                return &d;
        }
        return nullptr;
    }

    void suaCauHoi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can sua: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                d.hienThiCauHoi();
                if (d.danhSachCauHoi.empty())
                {
                    SetColor(RED);
                    cout << "De chua co cau hoi!\n";
                    SetColor(WHITE);
                    return;
                }
                int idCH = nhapSo<int>("Nhap ID cau hoi can sua: ");
                for (auto& ch : d.danhSachCauHoi)
                {
                    if (ch.id == idCH)
                    {
                        cout << "Noi dung moi: ";
                        getline(cin, ch.noiDung);
                        for (int i = 0; i < 4; i++)
                        {
                            cout << "Lua chon " << i + 1 << ": ";
                            getline(cin, ch.dapAn[i]);
                        }
                        do
                        {
                            ch.dapAnDung = nhapSo<int>("Dap an dung (1-4): ");
                        } while (ch.dapAnDung < 1 || ch.dapAnDung > 4);

                        SetColor(LIGHT_GREEN);
                        cout << "Sua cau hoi thanh cong!\n";
                        SetColor(YELLOW);
                        return;
                    }
                }
                SetColor(LIGHT_RED);
                cout << "Khong tim thay cau hoi!\n";
                SetColor(RED);
                return;
            }
        }
        SetColor(LIGHT_RED);
        cout << "Khong tim thay de thi!\n";
        SetColor(WHITE);
    }

    void xoaCauHoi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can xoa cau hoi: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                d.hienThiCauHoi();
                int idCH = nhapSo<int>("Nhap ID cau hoi can xoa: ");
                auto it = remove_if(d.danhSachCauHoi.begin(), d.danhSachCauHoi.end(), [idCH](const CauHoi& c)
                    { return c.id == idCH; });
                if (it != d.danhSachCauHoi.end())
                {
                    d.danhSachCauHoi.erase(it, d.danhSachCauHoi.end());
                    SetColor(GREEN);
                    cout << "Xoa cau hoi thanh cong!\n";
                    SetColor(WHITE);
                }
                else
                {
                    SetColor(RED);
                    cout << "Khong tim thay cau hoi!\n";
                    SetColor(WHITE);
                }
                return;
            }
        }

        SetColor(RED);
        cout << "Khong tim thay de thi!\n";
        SetColor(WHITE);
    }

    void menu(int teacherId, string monGiangVien, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms)
    {
        int ch;
        do
        {
            VeKhungTieuDe("QUAN LY DE THI", YELLOW);
            cout << "1. Tao de thi moi\n2. Sua cau hoi trong de\n3. Xoa cau hoi trong de\n4. Xem danh sach de thi\n5. Khoa/Mo de thi\n6. Xem lich thi\n0. Quay lai\n";
            ch = nhapSo<int>("Chon: ");
            switch (ch)
            {
            case 1:
                thietLapDeThi(teacherId, monGiangVien, classes, examRooms);
                break;
            case 2:
                suaCauHoi();
                break;
            case 3:
                xoaCauHoi();
                break;
            case 4:
                for (const auto& d : maDe)
                {
                    string loai = (d.loaiThi == 1) ? "[Chinh thuc]" : "[On tap]";
                    string soLan = "";
                    if (d.loaiThi == 2)
                    {
                        soLan = (d.soLanOnTap == -1)
                            ? " | So lan: Khong gioi han"
                            : " | So lan: " + to_string(d.soLanOnTap);
                    }
                    cout << "Ma de: " << d.id
                        << " | Mon: " << d.tenMon
                        << " " << loai
                        << soLan
                        << " | Thoi gian: " << d.thoiGianLamBai << "s"
                        << " | Trang thai: " << (d.locked ? "Khoa" : "Mo")
                        << endl;
                }
                break;
            case 5:
                khoaMoDeThi();
                break;
            case 6:
                xemLichThi();
                break;
            }
        } while (ch != 0);
    }
};

void QuanLyThi::saveDeThi() const
{
    ofstream fout("dethi.txt");
    for (const auto& de : maDe)
    {
        fout << de.id << '|' << de.tenMon << '|' << de.thoiGianLamBai << '|'
            << de.loaiThi << '|' << de.danhSachCauHoi.size()
            << '|' << de.phongThiId << '|' << formatDateTime(de.batDau)
            << '|' << formatDateTime(de.ketThuc) << '|' << (de.locked ? 1 : 0)
            << '|' << de.soLanOnTap << '|';

        for (size_t i = 0; i < de.dsLopDuocThi.size(); i++)
        {
            fout << de.dsLopDuocThi[i] << (i < de.dsLopDuocThi.size() - 1 ? "," : "");
        }
        if (de.dsLopDuocThi.empty())
            fout << "ALL";
        fout << '\n';

        for (const auto& ch : de.danhSachCauHoi)
        {
            fout << ch.id << '|' << ch.noiDung << '|' << ch.dapAn[0] << '|' << ch.dapAn[1] << '|' << ch.dapAn[2] << '|' << ch.dapAn[3] << '|' << ch.dapAnDung << '\n';
        }
    }
    fout.close();
}

void QuanLyThi::loadDeThi()
{
    maDe.clear();
    ifstream fin("dethi.txt");
    if (!fin)
        return;
    string line;
    while (getline(fin, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;
        stringstream ss(line);
        deThi de;
        string idStr, tgStr, loaiStr, soCauStr, phongThiStr,
            batDauStr, ketThucStr, lockedStr, soLanStr, dsLopStr;

        getline(ss, idStr, '|');
        getline(ss, de.tenMon, '|');
        getline(ss, tgStr, '|');
        getline(ss, loaiStr, '|');
        getline(ss, soCauStr, '|');
        getline(ss, phongThiStr, '|');
        getline(ss, batDauStr, '|');
        getline(ss, ketThucStr, '|');
        getline(ss, lockedStr, '|');
        getline(ss, soLanStr, '|'); // đọc soLanOnTap
        getline(ss, dsLopStr);

        de.id = stoi(idStr);
        de.thoiGianLamBai = stoi(tgStr);
        de.loaiThi = stoi(loaiStr);
        de.phongThiId = phongThiStr.empty() ? 0 : stoi(phongThiStr);
        de.batDau = batDauStr.empty() ? 0 : parseDateTime(batDauStr);
        de.ketThuc = ketThucStr.empty() ? 0 : parseDateTime(ketThucStr);
        de.locked = (lockedStr == "1");
        de.soLanOnTap = soLanStr.empty() ? -1 : stoi(soLanStr); // default -1

        if (dsLopStr != "ALL" && !dsLopStr.empty())
        {
            stringstream ssLop(dsLopStr);
            string tenLop;
            while (getline(ssLop, tenLop, ','))
            {
                de.dsLopDuocThi.push_back(tenLop);
            }
        }

        int soCau = stoi(soCauStr);
        for (int i = 0; i < soCau; i++)
        {
            getline(fin, line);
            stringstream s2(line);
            CauHoi ch;
            string idCH, dapAnDungStr;
            getline(s2, idCH, '|');
            getline(s2, ch.noiDung, '|');
            getline(s2, ch.dapAn[0], '|');
            getline(s2, ch.dapAn[1], '|');
            getline(s2, ch.dapAn[2], '|');
            getline(s2, ch.dapAn[3], '|');
            getline(s2, dapAnDungStr);
            ch.id = stoi(idCH);
            ch.dapAnDung = stoi(dapAnDungStr);
            de.danhSachCauHoi.push_back(ch);
        }
        maDe.push_back(de);
    }
    fin.close();
}

// ========================== STUDENT ==========================
class SinhVien : public Person
{
private:
    string lop;
    string gioitinh;
    string birthDay;
    string sdt;
    vector<KetQua> danhSachKetQua;
    double diemTB;
    int teacherId;

public:
    SinhVien() : Person()
    {
        lop = "";
        gioitinh = "";
        birthDay = "";
        sdt = "";
        diemTB = 0.0;
        teacherId = 0;
    }

    SinhVien(int id, string userN, string passW, string ten,
        string lop, string gioitinh, string birthDay, string sdt, double diemTB, int teacherId)
        : Person(id, userN, passW, ten), lop(lop), gioitinh(gioitinh), birthDay(birthDay), sdt(sdt), diemTB(diemTB), teacherId(teacherId)
    {
    }
    bool kiemTraLopDuocThi(const deThi& de) const
    {
        if (de.dsLopDuocThi.empty())
            return true; // Áp dụng toàn trường
        for (const string& l : de.dsLopDuocThi)
        {
            if (l == this->lop)
                return true;
        }
        return false;
    }
    string getRole() const override { return "SinhVien"; }
    string getLop() const { return lop; }
    string getGioiTinh() const { return gioitinh; }
    string getBirthDay() const { return birthDay; }
    string getSdt() const { return sdt; }
    int layMaGV() const { return teacherId; }
    double layDiem() const { return diemTB; }

    void datDiem(double d) { diemTB = d; }
    void setLop(const string& lop) { this->lop = lop; }
    void setGioiTinh(const string& gioitinh) { this->gioitinh = gioitinh; }
    void setBirthDay(const string& birthDay) { this->birthDay = birthDay; }
    void setSdt(const string& sdt) { this->sdt = sdt; }
    void setDiemTB(double diemTB) { this->diemTB = diemTB; }
    void setTeacherId(int teacherId) { this->teacherId = teacherId; }
    void setDanhSachKetQua(const vector<KetQua>& ds) { danhSachKetQua = ds; }
    const vector<KetQua>& getDanhSachKetQua() const { return danhSachKetQua; }

    void hienThiThongTin() const
    {
        cout << "==========Thong Tin Sinh Vien===========\n"
            << "[HS] " << fullName << " | Lop: " << lop << " | Gioi tinh: " << gioitinh
            << " | Birth: " << birthDay << " | SDT: " << sdt << " | Diem TB: " << diemTB
            << " | TeacherID: " << teacherId << endl;
    }

    void thongKeDiemTB()
    {
        double tong = 0;
        int dem = 0;
        for (const auto& kq : danhSachKetQua)
        {
            if (kq.loaiThi == 1)
            {
                tong += kq.diem;
                dem++;
            }
        }
        if (dem == 0)
        {
            cout << "Chua co diem chinh thuc nao!\n";
            return;
        }
        cout << "Diem TB (Chinh thuc): " << tong / dem << "\n";
    }

    void xemDanhSachOnTap(QuanLyThi& thi)
    {
        cout << "\n===== DANH SACH DE ON TAP =====\n";
        for (const auto& de : thi.getMaDe())
        {
            if (!kiemTraLopDuocThi(de))
                continue;
            if (de.loaiThi == 2)
            {
                cout << "Ma de: " << de.id << " | Mon: " << de.tenMon
                    << " | Thoi gian: " << de.thoiGianLamBai << " giay\n";
            }
        }
    }

    void datLaiMatKhau()
    {
        string mk;
        cout << "Nhap MK moi: ";
        getline(cin, mk);
        password = mk;
    }

    double traLoiCauHoi(vector<CauHoi> danhSachCauHoi, time_t start, int thoiGian) const
    {
        int n;
        double diemThi = 0;
        for (auto& ch : danhSachCauHoi)
        {
            time_t now = time(0);
            if (difftime(now, start) >= thoiGian)
            {
                cout << "Het gio lam bai!\n";
                break;
            }
            cout << "Cau " << ch.id << ". " << ch.noiDung << endl;
            for (int i = 0; i < 4; i++)
            {
                cout << i + 1 << ". " << ch.dapAn[i] << endl;
            }
            n = nhapSo<int>("Dap an ban chon: ");
            if (n == ch.dapAnDung)
            {
                diemThi += 10.0 / danhSachCauHoi.size();
            }
        }
        return diemThi;
    }

    void xemDanhSachDe(QuanLyThi& thi)
    {
        SetColor(LIGHT_CYAN);
        cout << "--- DANH SACH BAI THI CO THE LAM ---\n";
        SetColor(WHITE);

        bool coBai = false;

        for (const auto& de : thi.getMaDe())
        {
            if (!kiemTraLopDuocThi(de))
                continue;
            bool thiChinhThucRoi = false;
            if (de.loaiThi == 1)
            {
                for (const auto& kq : danhSachKetQua)
                {
                    if (kq.maDe == de.id && kq.loaiThi == 1)
                    {
                        thiChinhThucRoi = true;
                        break;
                    }
                }
            }
            if (!thiChinhThucRoi)
            {
                string loaiStr = (de.loaiThi == 1) ? "[CHINH THUC (1 lan)]" : "[ON TAP (n lan)]";
                cout << "Ma de: " << de.id << " | Mon: " << de.tenMon << " " << loaiStr << " | Thoi gian: " << de.thoiGianLamBai << "s\n";
                coBai = true;
            }
        }
        if (!coBai)
        {
            SetColor(YELLOW);
            cout << "Ban da lam het de Chinh thuc hien co!\n";
            SetColor(WHITE);
        }
    }

    void lamBaiThi(QuanLyThi& thi)
    {
        int chonDe = nhapSo<int>("Nhap Ma De muon thi: ");
        deThi* deHienTai = nullptr;
        for (auto& de : thi.getMaDe())
        {
            if (de.id == chonDe)
            {
                deHienTai = &de;
                break;
            }
        }

        if (deHienTai == nullptr || deHienTai->danhSachCauHoi.empty())
        {
            SetColor(RED);
            cout << "De khong hop le hoac chua co cau hoi!\n";
            SetColor(WHITE);
            return;
        }
        if (!kiemTraLopDuocThi(*deHienTai))
        {
            SetColor(RED);
            cout << "Ban khong thuoc danh sach lop duoc phep lam bai thi nay!\n";
            SetColor(WHITE);
            return;
        }
        if (deHienTai->locked)
        {
            SetColor(RED);
            cout << "De thi dang bi khoa!\n";
            SetColor(WHITE);
            return;
        }
        if (!deHienTai->isAvailable())
        {
            SetColor(YELLOW);
            cout << "De thi chua hoat dong. Lich thi: " << deHienTai->getSchedule() << "\n";
            SetColor(WHITE);
            return;
        }

        // Đếm số lần đã thi đề này
        int soLanDaThi = 0;
        for (const auto& kq : danhSachKetQua)
        {
            if (kq.maDe == deHienTai->id)
                soLanDaThi++;
        }

        if (deHienTai->loaiThi == 1)
        {
            // Chính thức: chỉ 1 lần
            if (soLanDaThi >= 1)
            {
                SetColor(RED);
                cout << "De thi Chinh thuc chi duoc lam 1 lan. Ban da thi roi!\n";
                SetColor(WHITE);
                return;
            }
        }
        else
        {
            // Ôn tập: kiểm tra giới hạn
            if (deHienTai->soLanOnTap != -1 && soLanDaThi >= deHienTai->soLanOnTap)
            {
                SetColor(RED);
                cout << "Ban da lam bai on tap nay " << soLanDaThi << "/"
                    << deHienTai->soLanOnTap << " lan. Het luot!\n";
                SetColor(WHITE);
                return;
            }

            SetColor(LIGHT_CYAN);
            if (deHienTai->soLanOnTap == -1)
                cout << "Lan thi thu " << soLanDaThi + 1 << " (Khong gioi han)\n";
            else
                cout << "Lan thi thu " << soLanDaThi + 1
                << "/" << deHienTai->soLanOnTap << "\n";
            SetColor(WHITE);
        }

        cout << "\n=== BAT DAU LAM BAI: "
            << deHienTai->tenMon
            << " ===\n";
        SetColor(WHITE);
        time_t start = time(0);

        double diemThi = traLoiCauHoi(
            deHienTai->danhSachCauHoi,
            start,
            deHienTai->thoiGianLamBai);

        int thoiGianThucTe =
            (int)difftime(time(0), start);
        if (thoiGianThucTe == 0)
            thoiGianThucTe = 1;
        SetColor(GREEN);
        cout << "=> Nop bai thanh cong! Diem: "
            << diemThi
            << " | Thoi gian: "
            << thoiGianThucTe
            << "s\n";

        SetColor(WHITE);
        KetQua kq;
        kq.idSV = this->id;
        kq.tenSV = this->fullName;
        kq.diem = diemThi;
        kq.mon = deHienTai->tenMon;
        kq.thoiGian = thoiGianThucTe;
        kq.maDe = deHienTai->id;
        kq.loaiThi = deHienTai->loaiThi;
        this->danhSachKetQua.push_back(kq);

        if (deHienTai->loaiThi == 2)
        {
            SetColor(LIGHT_CYAN);
            cout << "\n===== DAP AN =====\n";
            for (const auto& ch : deHienTai->danhSachCauHoi)
            {
                cout << ch.id << ". Dap an dung: " << ch.dapAnDung
                    << ". " << ch.dapAn[ch.dapAnDung - 1] << "\n";
            }
            SetColor(WHITE);
        }
    }

    void xemThongTinDeThi(QuanLyThi& thi)
    {
        int maDe = nhapSo<int>("Nhap ma de: ");
        for (auto& de : thi.getMaDe())
        {
            if (de.id == maDe)
            {
                cout << "\n===== THONG TIN DE THI =====\n";
                cout << "Mon: " << de.tenMon << endl;
                cout << "So cau hoi: " << de.danhSachCauHoi.size() << endl;
                cout << "Thoi gian: " << de.thoiGianLamBai << " giay\n";
                cout << "Loai thi: ";
                if (de.loaiThi == 1)
                    cout << "Chinh thuc\n";
                else
                    cout << "On tap\n";
                return;
            }
        }
        cout << "Khong tim thay de.\n";
    }

    void xemMucTieu()
    {
        VeKhungTieuDe("MUC TIEU PHAN DAU", GREEN);
        double tong = 0;
        int dem = 0;
        for (const auto& kq : danhSachKetQua)
        {
            if (kq.loaiThi == 1)
            {
                tong += kq.diem;
                dem++;
            }
        }
        double dtb = (dem == 0) ? 0 : (tong / dem);
        SetColor(BRIGHT_WHITE);
        cout << "Diem TB Tich Luy (Chinh thuc): "
            << dtb
            << "\n";

        SetColor(WHITE);

        if (dtb >= 8.0)
        {

            SetColor(GREEN);

            cout << "=> Xuat sac! Ban dat muc Gioi.\n";

            SetColor(WHITE);
        }
        else if (dtb >= 6.5)
        {

            SetColor(YELLOW);

            cout << "=> Kha. De dat Gioi can them: "
                << 8.0 - dtb
                << " diem.\n";

            SetColor(WHITE);
        }
        else
        {

            SetColor(RED);

            cout << "=> Yeu. De dat Kha can them: "
                << 6.5 - dtb
                << " diem.\n";

            SetColor(WHITE);
        }
    }
    void hienThiDanhSachKetQua(const vector<KetQua>& ds) const
    {
        if (ds.empty())
        {
            cout << "Chua co ket qua nao!\n";
            return;
        }
        for (const auto& kq : ds)
        {
            cout << "Ten SV: " << kq.tenSV << " | Mon: " << kq.mon << " | Diem: " << kq.diem << " | Thoi gian: " << kq.thoiGian << " giay" << endl;
        }
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName + "|" + lop + "|" + to_string(diemTB) + "|" + to_string(teacherId);
    }

    void menu(QuanLyThi& thi)
    {
        int choice;
        do
        {
            SetColor(CYAN);
            cout << "╔══════════════════════════════════════════════╗\n";
            cout << "║               MENU SINH VIEN                 ║\n";
            cout << "╠══════════════════════════════════════════════╣\n";

            SetColor(YELLOW);
            cout << "║  1. Xem thong tin ca nhan                    ║\n";
            cout << "║  2. Xem danh sach bai thi                    ║\n";
            cout << "║  3. Xem thong tin bai thi                    ║\n";
            cout << "║  4. Lam bai thi                              ║\n";
            cout << "║  5. Xem lich su thi                          ║\n";
            cout << "║  6. Thong ke diem trung binh                 ║\n";
            cout << "║  7. Xem danh sach bai on tap                 ║\n";
            cout << "║  8. Doi mat khau                             ║\n";
            cout << "║  0. Dang xuat                                ║\n";

            SetColor(CYAN);
            cout << "╚══════════════════════════════════════════════╝\n";

            SetColor(GREEN);
            choice = nhapSo<int>("Chon chuc nang: ");

            SetColor(WHITE);
            switch (choice)
            {
            case 1:
                SetColor(LIGHT_CYAN);
                hienThiThongTin();
                SetColor(WHITE);
                break;

            case 2:
                SetColor(LIGHT_BLUE);
                xemDanhSachDe(thi);
                SetColor(WHITE);
                break;

            case 3:
                SetColor(CYAN);
                xemThongTinDeThi(thi);
                SetColor(WHITE);
                break;

            case 4:
                SetColor(LIGHT_GREEN);
                lamBaiThi(thi);
                SetColor(WHITE);
                break;

            case 5:
                SetColor(YELLOW);
                hienThiDanhSachKetQua(danhSachKetQua);
                SetColor(WHITE);
                break;

            case 6:
                SetColor(LIGHT_MAGENTA);
                thongKeDiemTB();
                SetColor(WHITE);
                break;

            case 7:
                SetColor(LIGHT_YELLOW);
                xemDanhSachOnTap(thi);
                SetColor(WHITE);
                break;

            case 8:
                datLaiMatKhau();

                SetColor(GREEN);
                cout << "Doi mat khau thanh cong!\n";
                SetColor(WHITE);

                break;

            case 0:

                SetColor(GREEN);
                cout << "Dang xuat thanh cong!\n";
                SetColor(WHITE);

                break;

            default:

                SetColor(RED);
                cout << "Lua chon khong hop le!\n";
                SetColor(WHITE);
            }
        } while (choice != 0);
    }
};

// ========================== TEACHER ==========================
class GiangVien : public Person
{
private:
    string mon;

public:
    void setMon(const string& m) { mon = m; }
    string getMon() const { return mon; }

    GiangVien() : Person() {};
    GiangVien(int id, string u, string p, string ten, string mon) : Person(id, u, p, ten), mon(mon) {}

    string getRole() const override
    {
        return "GiangVien";
    }

    void hienThiThongTin() const
    {
        SetColor(LIGHT_YELLOW);
        cout << "[GV] " << fullName << " | Mon: " << mon << endl;

        SetColor(WHITE);
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName + "|" + mon;
    }

    void datLaiMatKhau()
    {
        string mk;

        SetColor(LIGHT_CYAN);
        cout << "Nhap MK moi: ";

        SetColor(WHITE);
        getline(cin, mk);

        password = mk;

        SetColor(LIGHT_GREEN);
        cout << "Doi mat khau thanh cong!\n";

        SetColor(WHITE);
    }
    void xemSinhVien(const vector<SinhVien>& students)
    {
        SetColor(LIGHT_CYAN);
        cout << "\n--- sinh vien LOP CHU NHIEM ---\n";
        SetColor(WHITE);
        for (const auto& hs : students)
        {
            if (hs.layMaGV() == this->id)
                hs.hienThiThongTin();
        }
    }

    void xemKetQuaThi(const vector<SinhVien>& students)
    {
        SetColor(LIGHT_CYAN);
        cout << "\n===== KET QUA THI LOP CHU NHIEM =====\n";

        for (const auto& hs : students)
        {
            if (hs.layMaGV() == this->id)
            {

                SetColor(LIGHT_YELLOW);
                cout << "\nSinh vien: " << hs.getFullName() << endl;

                for (const auto& kq : hs.getDanhSachKetQua())
                {

                    SetColor(LIGHT_GREEN);
                    cout << "Mon: " << kq.mon
                        << " | Diem: " << kq.diem
                        << " | Thoi gian: " << kq.thoiGian << "s\n";
                }
            }
        }

        SetColor(WHITE);
    }

    void sapXepSinhVienTheoDiem(vector<SinhVien>& students)
    {
        vector<SinhVien> ds;
        for (auto hs : students)
        {
            if (hs.layMaGV() == this->id)
            {
                ds.push_back(hs);
            }
        }
        sort(ds.begin(), ds.end(), [](SinhVien a, SinhVien b)
            { return a.layDiem() > b.layDiem(); });
        SetColor(CYAN);
        cout << "\n===== SAP XEP sinh vien THEO DIEM =====\n";
        SetColor(WHITE);
        for (auto& hs : ds)
        {
            cout << hs.getFullName() << " | Diem TB: " << hs.layDiem() << endl;
        }
    }

    void thongKe(const vector<SinhVien>& students)
    {
        double tong = 0;
        int dem = 0;
        for (const auto& hs : students)
        {
            if (hs.layMaGV() == this->id)
            {
                tong += hs.layDiem();
                dem++;
            }
        }
        if (dem)
            cout << "Diem TB: " << tong / dem << endl;
        else
            cout << "Khong co du lieu!\n";
    }

    void thongKeTheoMon(const vector<SinhVien>& students)
    {
        string monTK = inputLine("Nhap ten mon muon thong ke: ");
        SetColor(CYAN);

        cout << "\n--- THONG KE DIEM MON "
            << monTK
            << " (CHINH THUC) ---\n";

        SetColor(WHITE);
        struct HSData
        {
            string ten;
            double diem;
        };
        vector<HSData> tk;

        for (const auto& hs : students)
        {
            if (hs.layMaGV() == this->id)
            {
                double maxDiem = -1;
                for (const auto& kq : hs.getDanhSachKetQua())
                {
                    if (kq.mon == monTK && kq.loaiThi == 1)
                        maxDiem = max(maxDiem, kq.diem);
                }
                if (maxDiem >= 0)
                    tk.push_back({ hs.getFullName(), maxDiem });
            }
        }

        if (tk.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co du lieu thi Chinh thuc mon nay!\n";
            SetColor(WHITE);
        }
        else
        {
            sort(tk.begin(), tk.end(), [](const HSData& a, const HSData& b)
                { return a.diem > b.diem; });
            for (const auto& data : tk)
                cout << "HS: " << data.ten << " | Diem mon: " << data.diem << "\n";
        }
    }

    void menu(vector<SinhVien>& students, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms, QuanLyThi& thi)
    {
        int choice;
        do
        {
            SetColor(CYAN);
            cout << "╔══════════════════════════════════════════════╗\n";
            cout << "║               MENU GIANG VIEN                ║\n";
            cout << "╠══════════════════════════════════════════════╣\n";

            SetColor(YELLOW);
            cout << "║  1. Xem danh sach sinh vien                  ║\n";
            cout << "║  2. Thong ke diem trung binh                 ║\n";
            cout << "║  3. Quan ly de thi                           ║\n";
            cout << "║  4. Thong ke theo mon                        ║\n";
            cout << "║  5. Xem ket qua thi                          ║\n";
            cout << "║  6. Sap xep sinh vien theo diem              ║\n";
            cout << "║  7. Doi mat khau                             ║\n";
            cout << "║  0. Dang xuat                                ║\n";

            SetColor(CYAN);
            cout << "╚══════════════════════════════════════════════╝\n";

            SetColor(GREEN);
            choice = nhapSo<int>("Chon chuc nang: ");

            SetColor(WHITE);

            switch (choice)
            {
            case 1:
                SetColor(LIGHT_CYAN);
                xemSinhVien(students);
                SetColor(WHITE);
                break;

            case 2:
                SetColor(LIGHT_YELLOW);
                thongKe(students);
                SetColor(WHITE);
                break;

            case 3:
                SetColor(LIGHT_MAGENTA);
                thi.menu(this->id, this->mon, classes, examRooms);
                SetColor(WHITE);
                break;

            case 4:
                SetColor(LIGHT_BLUE);
                thongKeTheoMon(students);
                SetColor(WHITE);
                break;

            case 5:
                SetColor(LIGHT_GREEN);
                xemKetQuaThi(students);
                SetColor(WHITE);
                break;

            case 6:
                SetColor(YELLOW);
                sapXepSinhVienTheoDiem(students);
                SetColor(WHITE);
                break;

            case 7:
                datLaiMatKhau();
                SetColor(GREEN);
                cout << "Doi mat khau thanh cong!\n";
                SetColor(WHITE);
                break;

            case 0:
                SetColor(LIGHT_RED);
                cout << "Dang xuat thanh cong!\n";
                SetColor(WHITE);
                break;

            default:
                SetColor(RED);
                cout << "Lua chon khong hop le!\n";
                SetColor(WHITE);
            }

        } while (choice != 0);
    }
};

// ========================== QUAN LY USER ==========================
class QuanLyNguoiDung
{
private:
    vector<Person*> users;

public:
    void them(Person* p) { users.push_back(p); }

    Person* dangNhap()
    {
        string u, p;
        cout << "User: ";
        cin >> u;
        cout << "Pass: ";
        cin >> p;
        cin.ignore();

        for (auto user : users)
        {
            if (user->getUsername() == u && user->getPassword() == p)
            {
                return user;
            }
        }
        return NULL;
    }

    vector<Person*>& layDanhSach() { return users; }
};

// ========================== RANK ITEM ==========================
struct RankItem
{
    int id;
    string fullName;
    double score;
};

// ========================== ADMIN SYSTEM ==========================
class AdminSystem
{
private:
    Admin adminAccount;
    vector<GiangVien> teachers;
    vector<SinhVien> students;
    vector<ClassInfo> classes;
    vector<PhongThi> examRooms;
    vector<RankItem> ranking;
    int totalStudents;

public:
    vector<ClassInfo>& getClasses() { return classes; }
    vector<PhongThi>& getRooms() { return examRooms; }
    void sortRanking()
    {
        sort(ranking.begin(), ranking.end(), [](const RankItem& a, const RankItem& b)
            {
                if (a.score != b.score) return a.score > b.score;
                return a.fullName < b.fullName; });
    }

    AdminSystem() : totalStudents(0) { sortRanking(); }

    // ==================== VALIDATION ====================
    bool isValidName(const string& s) const
    {
        if (s.empty())
            return false;
        for (char c : s)
        {
            unsigned char uc = static_cast<unsigned char>(c);
            if (!(isalnum(uc) || isspace(uc)))
            {
                return false;
            }
        }
        return true;
    }

    bool isValidUsername(const string& s) const
    {
        if (s.empty())
            return false;
        for (char c : s)
        {
            unsigned char uc = static_cast<unsigned char>(c);
            if (!(isalnum(uc) || c == '_'))
            {
                return false;
            }
        }
        return true;
    }

    bool isValidPassword(const string& s) const
    {
        return s.length() >= 6;
    }

    bool teacherIdExists(int id) const
    {
        for (const auto& t : teachers)
        {
            if (t.getId() == id)
                return true;
        }
        return false;
    }

    bool studentIdExists(int id) const
    {
        for (const auto& hs : students)
        {
            if (hs.getId() == id)
                return true;
        }
        return false;
    }

    bool classIdExists(int id) const
    {
        for (const auto& c : classes)
        {
            if (c.id == id)
                return true;
        }
        return false;
    }

    bool usernameExists(const string& username) const
    {
        if (username == "admin")
            return true;
        for (const auto& t : teachers)
        {
            if (t.getUsername() == username)
                return true;
        }
        for (const auto& hs : students)
        {
            if (hs.getUsername() == username)
                return true;
        }
        return false;
    }

    int findTeacherIndexById(int id) const
    {
        for (int i = 0; i < (int)teachers.size(); i++)
        {
            if (teachers[i].getId() == id)
                return i;
        }
        return -1;
    }

    int findStudentIndexById(int id) const
    {
        for (int i = 0; i < (int)students.size(); i++)
        {
            if (students[i].getId() == id)
                return i;
        }
        return -1;
    }

    int findClassIndexById(int id) const
    {
        for (int i = 0; i < (int)classes.size(); i++)
        {
            if (classes[i].id == id)
                return i;
        }
        return -1;
    }

    bool teacherHasAssignedClass(int teacherId) const
    {
        for (const auto& c : classes)
        {
            if (c.teacherId == teacherId)
                return true;
        }
        return false;
    }

    void saveKetQua() const
    {
        ofstream fout("ketqua.txt");
        for (const auto& hs : students)
        {
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                fout << kq.idSV << '|' << kq.tenSV << '|' << kq.mon << '|' << kq.diem << '|' << kq.thoiGian << '|' << kq.maDe << '|' << kq.loaiThi << '\n';
            }
        }
        fout.close();
    }

    void loadKetQua()
    {
        ifstream fin("ketqua.txt");
        if (!fin)
            return;
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);
            string idSVStr, tenSV, mon, diemStr, thoiGianStr, maDeStr, loaiThiStr;

            getline(ss, idSVStr, '|');
            getline(ss, tenSV, '|');
            getline(ss, mon, '|');
            getline(ss, diemStr, '|');
            getline(ss, thoiGianStr, '|');
            getline(ss, maDeStr, '|');
            getline(ss, loaiThiStr);

            KetQua kq;
            kq.idSV = stoi(idSVStr);
            kq.tenSV = tenSV;
            kq.mon = mon;
            kq.diem = stod(diemStr);
            kq.thoiGian = stoi(thoiGianStr);
            kq.maDe = stoi(maDeStr);
            kq.loaiThi = stoi(loaiThiStr);

            for (auto& hs : students)
            {
                if (hs.getId() == kq.idSV)
                {
                    vector<KetQua> ds = hs.getDanhSachKetQua();
                    ds.push_back(kq);
                    hs.setDanhSachKetQua(ds);
                    break;
                }
            }
        }
        fin.close();
    }

    void recalcTotalStudents()
    {
        totalStudents = 0;
        for (const auto& c : classes)
        {
            totalStudents += c.studentCount;
        }
    }

    // ==================== LOGIN ====================
    bool loginAdmin()
    {
        SetColor(MAGENTA);

        cout << "╔════════════════════════════════════╗\n";
        cout << "║         DANG NHAP ADMIN            ║\n";
        cout << "╚════════════════════════════════════╝\n";

        string user, pass;

        SetColor(CYAN);
        cout << "Username: ";
        SetColor(WHITE);
        getline(cin, user);

        SetColor(CYAN);
        cout << "Password: ";
        SetColor(WHITE);
        getline(cin, pass);

        if (adminAccount.login(user, pass))
        {
            SetColor(GREEN);
            cout << "\nDang nhap thanh cong.\n";

            SetColor(WHITE);
            return true;
        }

        SetColor(RED);
        cout << "\nSai username hoac password.\n";

        SetColor(WHITE);
        return false;
    }

    //==================== STUDENT CRUD =====================
    void addStudent()
    {
        cout << "\n===== THEM SINH VIEN =====\n";
        SinhVien hs;
        hs.setId(nhapSo<int>("Nhap ID sinh vien: "));

        if (studentIdExists(hs.getId()))
        {
            cout << "ID sinh vien da ton tai.\n";
            return;
        }

        hs.setUsername(inputLine("Nhap username: "));
        if (!isValidUsername(hs.getUsername()))
        {
            cout << "Username khong hop le. Chi gom chu, so, dau _.\n";
            return;
        }
        if (usernameExists(hs.getUsername()))
        {
            cout << "Username da ton tai.\n";
            return;
        }

        hs.setPassword(inputLine("Nhap password: "));
        if (!isValidPassword(hs.getPassword()))
        {
            cout << "Password khong hop le. Toi thieu 6 ky tu.\n";
            return;
        }

        hs.setFullName(inputLine("Nhap ho ten: "));
        if (!isValidName(hs.getFullName()))
        {
            cout << "Ho ten khong hop le.\n";
            return;
        }

        hs.setLop(inputLine("Nhap lop: "));
        hs.setGioiTinh(inputLine("Nhap gioi tinh: "));
        hs.setBirthDay(inputLine("Nhap ngay sinh: "));
        hs.setSdt(inputLine("Nhap so dien thoai: "));
        hs.setDiemTB(nhapSo<double>("Nhap diem trung binh: "));

        int teacherId = nhapSo<int>("Nhap ID giang vien phu trach: ");
        if (!teacherIdExists(teacherId))
        {
            cout << "ID giang vien khong ton tai.\n";
            return;
        }

        hs.setTeacherId(teacherId);
        students.push_back(hs);
        cout << "Them sinh vien thanh cong.\n";
    }

    void viewStudents() const
    {
        cout << "\n===== DANH SACH SINH VIEN =====\n";
        if (students.empty())
        {
            cout << "Chua co sinh vien nao.\n";
            return;
        }
        for (int i = 0; i < (int)students.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID: " << students[i].getId() << " | Username: " << students[i].getUsername() << " | Full name: " << students[i].getFullName() << '\n';
        }
    }

    void editStudent()
    {
        cout << "\n===== SUA SINH VIEN =====\n";
        int id = nhapSo<int>("Nhap ID sinh vien can sua: ");
        int idx = findStudentIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay sinh vien.\n";
            return;
        }
        int choice = nhapSo<int>("Chon: ");

        if (choice == 1 || choice == 4)
        {
            string newUsername = inputLine("Nhap username moi: ");
            if (!isValidUsername(newUsername))
            {
                cout << "Username khong hop le.\n";
                return;
            }
            if (newUsername != students[idx].getUsername() && usernameExists(newUsername))
            {
                cout << "Username da ton tai.\n";
                return;
            }
            students[idx].setUsername(newUsername);
        }

        if (choice == 2 || choice == 4)
        {
            string newPassword = inputLine("Nhap password moi: ");
            if (!isValidPassword(newPassword))
            {
                cout << "Password khong hop le.\n";
                return;
            }
            students[idx].setPassword(newPassword);
        }

        if (choice == 3 || choice == 4)
        {
            string newFullName = inputLine("Nhap ho ten moi: ");
            if (!isValidName(newFullName))
            {
                cout << "Ho ten khong hop le.\n";
                return;
            }
            students[idx].setFullName(newFullName);
        }

        cout << "1. Sua username\n2. Sua password\n3. Sua ho ten\n4. Sua lop\n5. Sua gioi tinh\n6. Sua ngay sinh\n7. Sua so dien thoai\n8. Sua diem trung binh\n9. Sua giang vien phu trach\n10. Sua tat ca\n";
        int chHS = nhapSo<int>("Chon: ");

        if (chHS == 1 || chHS == 10)
        {
            string newUsername = inputLine("Nhap username moi: ");
            if (!isValidUsername(newUsername))
            {
                cout << "Username khong hop le.\n";
                return;
            }
            if (newUsername != students[idx].getUsername() && usernameExists(newUsername))
            {
                cout << "Username da ton tai.\n";
                return;
            }
            students[idx].setUsername(newUsername);
        }
        if (chHS == 2 || chHS == 10)
        {
            string newPassword = inputLine("Nhap password moi: ");
            if (!isValidPassword(newPassword))
            {
                cout << "Password khong hop le.\n";
                return;
            }
            students[idx].setPassword(newPassword);
        }
        if (chHS == 3 || chHS == 10)
        {
            string newFullName = inputLine("Nhap ho ten moi: ");
            if (!isValidName(newFullName))
            {
                cout << "Ho ten khong hop le.\n";
                return;
            }
            students[idx].setFullName(newFullName);
        }
        if (chHS == 4 || chHS == 10)
        {
            string newClass = inputLine("Nhap lop moi: ");
            students[idx].setLop(newClass);
        }
        if (chHS == 5 || chHS == 10)
        {
            string newGender = inputLine("Nhap gioi tinh moi: ");
            students[idx].setGioiTinh(newGender);
        }
        if (chHS == 6 || chHS == 10)
        {
            string newBirthDay = inputLine("Nhap ngay sinh moi: ");
            students[idx].setBirthDay(newBirthDay);
        }
        if (chHS == 7 || chHS == 10)
        {
            string newPhone = inputLine("Nhap so dien thoai moi: ");
            students[idx].setSdt(newPhone);
        }
        if (chHS == 8 || chHS == 10)
        {
            double newDiemTB = nhapSo<double>("Nhap diem trung binh moi: ");
            students[idx].setDiemTB(newDiemTB);
        }
        if (chHS == 9 || chHS == 10)
        {
            int newTeacherId = nhapSo<int>("Nhap ID giang vien moi: ");
            if (!teacherIdExists(newTeacherId))
            {
                cout << "ID giang vien khong ton tai.\n";
                return;
            }
            students[idx].setTeacherId(newTeacherId);
        }
        cout << "Sua sinh vien thanh cong.\n";
    }

    void deleteStudent()
    {
        cout << "\n===== XOA SINH VIEN =====\n";
        int id = nhapSo<int>("Nhap ID sinh vien can xoa: ");
        int idx = findStudentIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay sinh vien.\n";
            return;
        }
        students.erase(students.begin() + idx);
        cout << "Xoa sinh vien thanh cong.\n";
    }

    vector<SinhVien>& getStudents() { return students; }

    // ==================== TEACHER CRUD ====================
    void addTeacher()
    {
        cout << "\n===== THEM GIANG VIEN =====\n";
        GiangVien t;
        t.setId(nhapSo<int>("Nhap ID giang vien: "));
        if (teacherIdExists(t.getId()))
        {
            cout << "ID giang vien da ton tai.\n";
            return;
        }

        t.setUsername(inputLine("Nhap username: "));
        if (!isValidUsername(t.getUsername()))
        {
            cout << "Username khong hop le.\n";
            return;
        }
        if (usernameExists(t.getUsername()))
        {
            cout << "Username da ton tai.\n";
            return;
        }

        t.setPassword(inputLine("Nhap password: "));
        if (!isValidPassword(t.getPassword()))
        {
            cout << "Password phai >= 6 ky tu.\n";
            return;
        }

        t.setFullName(inputLine("Nhap ho ten: "));
        string mon = inputLine("Nhap mon day: ");
        t.setMon(mon);

        teachers.push_back(t);
        cout << "Them giang vien thanh cong.\n";
    }

    void viewTeachers() const
    {
        cout << "\n===== DANH SACH GIANG VIEN =====\n";
        if (teachers.empty())
        {
            cout << "Chua co giang vien nao.\n";
            return;
        }
        for (int i = 0; i < teachers.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID: " << teachers[i].getId() << " | Username: " << teachers[i].getUsername() << " | Full Name: " << teachers[i].getFullName() << " | Mon: " << teachers[i].getMon() << '\n';
        }
    }

    void editTeacher()
    {
        cout << "\n===== SUA GIANG VIEN =====\n";
        int id = nhapSo<int>("Nhap ID giang vien can sua: ");
        int idx = findTeacherIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay giang vien.\n";
            return;
        }

        cout << "1. Sua username\n2. Sua password\n3. Sua ho ten\n4. Sua tat ca\n";
        int choice = nhapSo<int>("Chon: ");

        if (choice == 1 || choice == 4)
        {
            string newUsername = inputLine("Nhap username moi: ");
            if (!isValidUsername(newUsername))
            {
                cout << "Username khong hop le.\n";
                return;
            }
            if (newUsername != teachers[idx].getUsername() && usernameExists(newUsername))
            {
                cout << "Username da ton tai.\n";
                return;
            }
            teachers[idx].setUsername(newUsername);
        }

        if (choice == 2 || choice == 4)
        {
            string newPassword = inputLine("Nhap password moi: ");
            if (!isValidPassword(newPassword))
            {
                cout << "Password khong hop le.\n";
                return;
            }
            teachers[idx].setPassword(newPassword);
        }

        if (choice == 3 || choice == 4)
        {
            string newFullName = inputLine("Nhap ho ten moi: ");
            if (!isValidName(newFullName))
            {
                cout << "Ho ten khong hop le.\n";
                return;
            }
            teachers[idx].setFullName(newFullName);
        }

        cout << "Sua giang vien thanh cong.\n";
    }

    void deleteTeacher()
    {
        cout << "\n===== XOA GIANG VIEN =====\n";
        int id = nhapSo<int>("Nhap ID giang vien can xoa: ");
        int idx = findTeacherIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay giang vien.\n";
            return;
        }
        if (teacherHasAssignedClass(id))
        {
            cout << "Khong the xoa. giang vien dang duoc gan cho lop.\n";
            return;
        }

        teachers.erase(teachers.begin() + idx);
        cout << "Xoa giang vien thanh cong.\n";
    }

    vector<GiangVien>& getTeachers() { return teachers; }

    // ==================== CLASS CRUD ====================
    void addClass()
    {
        cout << "\n===== THEM LOP =====\n";
        ClassInfo c;
        c.id = nhapSo<int>("Nhap ID lop: ");
        if (classIdExists(c.id))
        {
            cout << "ID lop da ton tai.\n";
            return;
        }

        c.className = inputLine("Nhap ten lop: ");
        if (c.className.empty())
        {
            cout << "Ten lop khong duoc rong.\n";
            return;
        }

        c.coVanHocTapId = nhapSo<int>("Nhap ID Co van hoc tap (0 neu chua gan): ");
        if (c.coVanHocTapId != 0 && !teacherIdExists(c.coVanHocTapId))
        {
            cout << "Giang vien khong ton tai.\n";
            return;
        }

        int soGV = nhapSo<int>("Nhap so luong giao vien giang day lop nay: ");
        for (int i = 0; i < soGV; i++)
        {
            int idGV = nhapSo<int>("Nhap ID giao vien thu " + to_string(i + 1) + ": ");
            if (teacherIdExists(idGV))
            {
                c.danhSachGiangVien.push_back(idGV);
            }
            else
            {
                SetColor(YELLOW);
                cout << "ID Giang vien khong ton tai. Da bo qua.\n";
                SetColor(WHITE);
            }
        }

        c.studentCount = nhapSo<int>("Nhap so luong sinh vien: ");
        if (c.studentCount < 0)
        {
            cout << "So luong sinh vien khong hop le.\n";
            return;
        }

        classes.push_back(c);
        recalcTotalStudents();
        cout << "Them lop thanh cong.\n";
    }

    void viewClasses() const
    {
        cout << "\n===== DANH SACH LOP =====\n";
        if (classes.empty())
        {
            cout << "Chua co lop nao.\n";
            return;
        }
        for (int i = 0; i < (int)classes.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID lop: " << classes[i].id
                << " | Ten lop: " << classes[i].className
                << " | CVHT ID: " << classes[i].coVanHocTapId
                << " | So GV day: " << classes[i].danhSachGiangVien.size()
                << " | So SV: " << classes[i].studentCount << '\n';
        }
    }
    void editClass()
    {
        cout << "\n===== SUA LOP =====\n";
        int id = nhapSo<int>("Nhap ID lop can sua: ");
        int idx = findClassIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay lop.\n";
            return;
        }

        cout << "1. Sua ten lop\n2. Sua giang vien phu trach\n3. Sua so luong sinh vien\n4. Sua tat ca\n";
        int choice = nhapSo<int>("Chon: ");

        if (choice == 1 || choice == 4)
        {
            string newClassName = inputLine("Nhap ten lop moi: ");
            classes[idx].className = newClassName;
        }

        if (choice == 2 || choice == 4)
        {
            int newTeacherId = nhapSo<int>("Nhap ID giang vien moi (0 neu bo gan): ");
            if (newTeacherId != 0 && !teacherIdExists(newTeacherId))
            {
                cout << "giang vien khong ton tai.\n";
                return;
            }
            classes[idx].teacherId = newTeacherId;
        }

        if (choice == 3 || choice == 4)
        {
            int newStudentCount = nhapSo<int>("Nhap so luong sinh vien moi: ");
            if (newStudentCount < 0)
            {
                cout << "So luong sinh vien khong hop le.\n";
                return;
            }
            classes[idx].studentCount = newStudentCount;
            recalcTotalStudents();
        }

        cout << "Sua lop thanh cong.\n";
    }

    void deleteClass()
    {
        cout << "\n===== XOA LOP =====\n";
        int id = nhapSo<int>("Nhap ID lop can xoa: ");
        int idx = findClassIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay lop.\n";
            return;
        }

        classes.erase(classes.begin() + idx);
        recalcTotalStudents();
        cout << "Xoa lop thanh cong.\n";
    }

    // ==================== STATISTICS ====================
    void countUsers() const
    {
        cout << "\n===== SO LUONG NGUOI DUNG =====\n";
        cout << "So giang vien: " << teachers.size() << '\n';
        cout << "So sinh vien: " << students.size() << '\n';
    }

    bool roomIdExists(int id) const
    {
        for (const auto& r : examRooms)
        {
            if (r.id == id)
                return true;
        }
        return false;
    }

    int findRoomIndexById(int id) const
    {
        for (int i = 0; i < (int)examRooms.size(); i++)
        {
            if (examRooms[i].id == id)
                return i;
        }
        return -1;
    }

    void addRoom()
    {
        cout << "\n===== THEM PHONG THI =====\n";
        PhongThi r;
        r.id = nhapSo<int>("Nhap ID phong thi: ");
        if (roomIdExists(r.id))
        {
            cout << "ID phong thi da ton tai.\n";
            return;
        }
        r.tenPhong = inputLine("Nhap ten phong: ");
        r.sucChua = nhapSo<int>("Nhap suc chua: ");
        r.locked = false;
        examRooms.push_back(r);
        cout << "Them phong thi thanh cong.\n";
    }

    void viewRooms() const
    {
        cout << "\n===== DANH SACH PHONG THI =====\n";
        if (examRooms.empty())
        {
            cout << "Chua co phong thi nao.\n";
            return;
        }
        for (int i = 0; i < (int)examRooms.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID: " << examRooms[i].id << " | Ten phong: " << examRooms[i].tenPhong
                << " | Suc chua: " << examRooms[i].sucChua
                << " | Trang thai: " << (examRooms[i].locked ? "Khoa" : "Mo") << '\n';
        }
    }

    void editRoom()
    {
        cout << "\n===== SUA PHONG THI =====\n";
        int id = nhapSo<int>("Nhap ID phong thi can sua: ");
        int idx = findRoomIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay phong thi.\n";
            return;
        }
        cout << "1. Sua ten phong\n2. Sua suc chua\n3. Khoa/Mo phong\n4. Sua tat ca\n";
        int choice = nhapSo<int>("Chon: ");
        if (choice == 1 || choice == 4)
        {
            examRooms[idx].tenPhong = inputLine("Nhap ten phong moi: ");
        }
        if (choice == 2 || choice == 4)
        {
            examRooms[idx].sucChua = nhapSo<int>("Nhap suc chua moi: ");
        }
        if (choice == 3 || choice == 4)
        {
            examRooms[idx].locked = !examRooms[idx].locked;
            cout << "Phong thi da " << (examRooms[idx].locked ? "duoc khoa" : "duoc mo") << ".\n";
        }
        cout << "Sua phong thi thanh cong.\n";
    }

    void deleteRoom()
    {
        cout << "\n===== XOA PHONG THI =====\n";
        int id = nhapSo<int>("Nhap ID phong thi can xoa: ");
        int idx = findRoomIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay phong thi.\n";
            return;
        }
        examRooms.erase(examRooms.begin() + idx);
        cout << "Xoa phong thi thanh cong.\n";
    }

    void manageExamRooms()
    {
        int choice;
        do
        {
            cout << "\n===== QUAN LY PHONG THI =====\n";
            cout << "1. Them phong thi\n2. Sua phong thi\n3. Xoa phong thi\n4. Xem phong thi\n0. Quay lai\n";
            choice = nhapSo<int>("Chon: ");
            switch (choice)
            {
            case 1:
                addRoom();
                break;
            case 2:
                editRoom();
                break;
            case 3:
                deleteRoom();
                break;
            case 4:
                viewRooms();
                break;
            }
        } while (choice != 0);
    }

    void exportExamReport(const QuanLyThi& thi) const
    {
        ofstream fout("bao_cao_thi.csv");
        fout << "ID_SV,Ten_SV,Ma_De,Mon,Loai_Thi,Điểm,Thoi_gian,Phong,Thoi_gian_bat_dau,Thoi_gian_ket_thuc,Trang_thai\n";
        for (const auto& hs : students)
        {
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                const deThi* de = thi.timDeThi(kq.maDe);
                string phong = de ? to_string(de->phongThiId) : "0";
                string batDau = de ? formatDateTime(de->batDau) : "N/A";
                string ketThuc = de ? formatDateTime(de->ketThuc) : "N/A";
                string trangThai = de ? (de->locked ? "Khoa" : (de->isAvailable() ? "Dang mo" : "Chua den gio/da ket thuc")) : "Khong co";
                fout << hs.getId() << ',' << hs.getFullName() << ',' << kq.maDe << ',' << kq.mon << ',' << kq.loaiThi << ',' << kq.diem << ',' << kq.thoiGian << ','
                    << phong << ',' << batDau << ',' << ketThuc << ',' << trangThai << '\n';
            }
        }
        fout.close();
        cout << "Xuat bao cao thi thanh cong: bao_cao_thi.csv\n";
    }

    void viewExamStatistics(const QuanLyThi& thi) const
    {
        cout << "\n===== THONG KE BAO CAO THI =====\n";
        if (students.empty())
        {
            cout << "Chua co sinh vien!\n";
            return;
        }
        map<int, vector<double>> examScores;
        for (const auto& hs : students)
        {
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                examScores[kq.maDe].push_back(kq.diem);
            }
        }
        if (examScores.empty())
        {
            cout << "Chua co ket qua thi de thong ke.\n";
            return;
        }
        for (const auto& exam : examScores)
        {
            const deThi* de = thi.timDeThi(exam.first);
            if (!de)
                continue;
            const auto& scores = exam.second;
            double sum = 0;
            double maxScore = 0;
            double minScore = 10;
            for (double v : scores)
            {
                sum += v;
                maxScore = max(maxScore, v);
                minScore = min(minScore, v);
            }
            double avg = sum / scores.size();
            cout << "Ma de: " << de->id << " | Mon: " << de->tenMon << " | So luot thi: " << scores.size() << " | Diem TB: " << avg << " | Min: " << minScore << " | Max: " << maxScore << '\n';
            int barLength = min((int)scores.size(), 20);
            cout << "Bang diem: ";
            for (int i = 0; i < barLength; i++)
                cout << "#";
            cout << "\n";
        }
    }

    void saveRooms() const
    {
        ofstream fout("phongthi.txt");
        for (const auto& r : examRooms)
        {
            fout << r.id << '|' << r.tenPhong << '|' << r.sucChua << '|' << (r.locked ? 1 : 0) << '\n';
        }
    }

    void loadRooms(const string& fileName)
    {
        examRooms.clear();
        ifstream fin(fileName);
        if (!fin)
        {
            cout << "Khong mo duoc file phong thi!\n";
            return;
        }
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);
            string idStr, tenPhong, sucChuaStr, lockedStr;
            getline(ss, idStr, '|');
            getline(ss, tenPhong, '|');
            getline(ss, sucChuaStr, '|');
            getline(ss, lockedStr);
            PhongThi r;
            r.id = stoi(idStr);
            r.tenPhong = tenPhong;
            r.sucChua = stoi(sucChuaStr);
            r.locked = (lockedStr == "1");
            examRooms.push_back(r);
        }
        fin.close();
        cout << "Tai danh sach phong thi thanh cong.\n";
    }

    void viewSystemStatistics() const
    {
        cout << "\n===== THONG KE TOAN HE THONG =====\n";
        cout << "So giang vien: " << teachers.size() << '\n';
        cout << "So lop: " << classes.size() << '\n';
        cout << "So phong thi: " << examRooms.size() << '\n';
        cout << "Tong so sinh vien: " << totalStudents << '\n';
        cout << "So muc trong bang xep hang: " << ranking.size() << '\n';

        int assignedClassCount = 0;
        for (const auto& c : classes)
        {
            if (c.teacherId != 0)
                assignedClassCount++;
        }
        cout << "So lop da gan giang vien: " << assignedClassCount << '\n';
    }

    void viewRanking()
    {
        SetColor(CYAN);
        cout << "\n===== BANG XEP HANG TOAN TRUONG (CHINH THUC) =====\n";
        SetColor(WHITE);
        struct HSScore
        {
            string fullName;
            double score;
        };
        vector<HSScore> bxh;

        for (const auto& hs : students)
        {
            double tong = 0;
            int dem = 0;
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                if (kq.loaiThi == 1)
                {
                    tong += kq.diem;
                    dem++;
                }
            }
            if (dem > 0)
                bxh.push_back({ hs.getFullName(), tong / dem });
        }

        if (bxh.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co du lieu!\n";
            SetColor(WHITE);
            return;
        }
        sort(bxh.begin(), bxh.end(), [](const HSScore& a, const HSScore& b)
            { return a.score > b.score; });
        for (int i = 0; i < bxh.size(); i++)
        {
            cout << i + 1 << ". Ho ten: " << bxh[i].fullName << " | Diem TB: " << bxh[i].score << '\n';
        }
    }

    // ==================== SAVE / LOAD ====================
    void saveTeachers() const
    {
        ofstream fout("giangvien.txt");
        for (const auto& t : teachers)
        {
            fout << t.getId() << '|' << t.getUsername() << '|' << t.getPassword() << '|' << t.getFullName() << '|' << t.getMon() << '\n';
        }
        fout.close();
    }

    void saveRanking() const
    {
        ofstream fout("ranking.txt");
        for (const auto& r : ranking)
        {
            fout << r.id << '|' << r.fullName << '|' << r.score << '\n';
        }
    }

    void saveStudents() const
    {
        ofstream file("sinhvien.txt");
        for (const auto& s : students)
        {
            file << s.getId() << '|' << s.getUsername() << '|' << s.getPassword() << '|' << s.getFullName() << '|' << s.getLop() << '|' << s.getGioiTinh() << '|' << s.getBirthDay() << '|' << s.getSdt() << '|' << s.layDiem() << '|' << s.layMaGV() << '\n';
        }
        file.close();
    }

    void saveData() const
    {
        saveStudents();
        saveTeachers();
        saveClasses();
        saveRooms();
        saveRanking();
        saveKetQua();
        cout << "Luu du lieu thanh cong.\n";
    }

    void loadStudents(const string& fileName)
    {
        students.clear();
        ifstream fin(fileName);
        if (!fin)
        {
            cout << "Khong mo duoc file sinh vien!\n";
            return;
        }

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);

            string idStr, username, password, fullName, mon;
            string lop, gioitinh, birthDay, sdt;
            string diemTBStr, teacherIdStr;

            getline(ss, idStr, '|');
            getline(ss, username, '|');
            getline(ss, password, '|');
            getline(ss, fullName, '|');
            getline(ss, lop, '|');
            getline(ss, gioitinh, '|');
            getline(ss, birthDay, '|');
            getline(ss, sdt, '|');
            getline(ss, diemTBStr, '|');
            getline(ss, teacherIdStr);

            SinhVien s;
            s.setId(stoi(idStr));
            s.setUsername(username);
            s.setPassword(password);
            s.setFullName(fullName);
            s.setLop(lop);
            s.setGioiTinh(gioitinh);
            s.setBirthDay(birthDay);
            s.setSdt(sdt);

            if (diemTBStr.empty())
                diemTBStr = "0";
            if (teacherIdStr.empty())
                teacherIdStr = "0";

            s.setDiemTB(stod(diemTBStr));
            s.setTeacherId(stoi(teacherIdStr));
            students.push_back(s);
        }
        fin.close();
        cout << "Tai danh sach sinh vien thanh cong.\n";
    }

    void loadTeachers(const string& fileName)
    {
        teachers.clear();
        ifstream fin(fileName);
        if (!fin)
        {
            cout << "Khong mo duoc file giang vien!\n";
            return;
        }

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);
            string idStr, username, password, fullName, mon;

            getline(ss, idStr, '|');
            getline(ss, username, '|');
            getline(ss, password, '|');
            getline(ss, fullName, '|');
            getline(ss, mon);

            GiangVien t;
            t.setId(stoi(idStr));
            t.setUsername(username);
            t.setPassword(password);
            t.setFullName(fullName);
            t.setMon(mon);
            teachers.push_back(t);
        }
        fin.close();
        cout << "Tai danh sach giang vien thanh cong.\n";
    }

    void saveClasses() const
    {
        ofstream fout("lophoc.txt");
        for (const auto& c : classes)
        {
            fout << c.id << '|'
                << c.className << '|'
                << c.coVanHocTapId << '|'
                << c.teacherId << '|'
                << c.studentCount << '\n';
        }
        fout.close();
    }

    void loadClasses(const string& fileName)
    {
        classes.clear();
        ifstream fin(fileName);
        if (!fin)
            return;
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);
            string idStr, className, cvhtIdStr, teacherIdStr, studentCountStr;

            getline(ss, idStr, '|');
            getline(ss, className, '|');
            getline(ss, cvhtIdStr, '|');
            getline(ss, teacherIdStr, '|'); // cột 4 = teacherId
            getline(ss, studentCountStr);   // cột 5 = studentCount

            ClassInfo c;
            c.id = stoi(idStr);
            c.className = className;
            c.coVanHocTapId = stoi(cvhtIdStr);
            c.teacherId = stoi(teacherIdStr);
            c.studentCount = stoi(studentCountStr);

            // Đồng bộ danhSachGiangVien từ teacherId
            if (c.teacherId != 0)
                c.danhSachGiangVien.push_back(c.teacherId);

            classes.push_back(c);
        }
        recalcTotalStudents();
        fin.close();
    }

    void loadRanking()
    {
        ranking.clear();
        ifstream fin("ranking.txt");
        if (!fin)
            return;

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (line.empty())
                continue;
            stringstream ss(line);
            string idStr, fullName, scoreStr;

            getline(ss, idStr, '|');
            getline(ss, fullName, '|');
            getline(ss, scoreStr);

            RankItem r;
            r.id = stoi(idStr);
            r.fullName = fullName;
            r.score = stod(scoreStr);
            ranking.push_back(r);
        }
        sortRanking();
    }

    void loadData()
    {
        cout << "Chon file sinh vien...\n";
        string studentFile = taiFile();
        if (!studentFile.empty())
        {
            loadStudents(studentFile);
        }
        else
        {
            cout << "Khong chon file sinh vien!\n";
        }

        cout << "Chon file giang vien...\n";
        string teacherFile = taiFile();
        if (!teacherFile.empty())
        {
            loadTeachers(teacherFile);
        }
        else
        {
            cout << "Khong chon file giang vien!\n";
        }

        cout << "Chon file lop hoc...\n";
        string classFile = taiFile();
        if (!classFile.empty())
        {
            loadClasses(classFile);
        }
        else
        {
            cout << "Khong chon file lop hoc!\n";
        }

        cout << "Chon file phong thi...\n";
        string roomFile = taiFile();
        if (!roomFile.empty())
        {
            loadRooms(roomFile);
        }
        else
        {
            cout << "Khong chon file phong thi!\n";
        }
        loadRanking();
        loadKetQua();
        recalcTotalStudents();
        cout << "Tai du lieu thanh cong.\n";
    }

    // ==================== SEARCH & FILTER ====================
    void searchStudent()
    {
        cout << "\n===== TIM KIEM SINH VIEN =====\n";
        cout << "1. Tim theo ID\n2. Tim theo ten\n3. Tim theo lop\n";
        int option = nhapSo<int>("Chon tieu chi: ");

        if (option == 1)
        {
            int id = nhapSo<int>("Nhap ID: ");
            bool found = false;
            for (const auto& s : students)
            {
                if (s.getId() == id)
                {
                    cout << "ID: " << s.getId() << " | Ten: " << s.getFullName() << " | Lop: " << s.getLop() << " | Diem TB: " << s.layDiem() << '\n';
                    found = true;
                    break;
                }
            }
            if (!found)
                cout << "Khong tim thay sinh vien.\n";
        }
        else if (option == 2)
        {
            string name = inputLine("Nhap ten (phan hay day du): ");
            bool found = false;
            for (const auto& s : students)
            {
                if (s.getFullName().find(name) != string::npos)
                {
                    cout << "ID: " << s.getId() << " | Ten: " << s.getFullName() << " | Lop: " << s.getLop() << " | Diem TB: " << s.layDiem() << '\n';
                    found = true;
                }
            }
            if (!found)
                cout << "Khong tim thay sinh vien.\n";
        }
        else if (option == 3)
        {
            string lop = inputLine("Nhap lop: ");
            bool found = false;
            for (const auto& s : students)
            {
                if (s.getLop() == lop)
                {
                    cout << "ID: " << s.getId() << " | Ten: " << s.getFullName() << " | Diem TB: " << s.layDiem() << '\n';
                    found = true;
                }
            }
            if (!found)
                cout << "Khong tim thay sinh vien lop nay.\n";
        }
        else
            cout << "Lua chon khong hop le.\n";
    }
    void filterExams(QuanLyThi& thi)
    {
        SetColor(LIGHT_CYAN);
        cout << "\n===== LOC DE THI =====\n";
        SetColor(WHITE);
        cout << "1. Loc theo mon\n2. Loc theo loai thi\n3. Loc theo trang thai (khoa/mo)\n";
        int option = nhapSo<int>("Chon tieu chi: ");

        if (option == 1)
        {
            string mon = inputLine("Nhap ten mon: ");
            bool found = false;
            for (const auto& de : thi.getMaDe())
            {
                if (de.tenMon.find(mon) != string::npos)
                {
                    cout << "ID: " << de.id << " | Mon: " << de.tenMon
                        << " | Loai: " << (de.loaiThi == 1 ? "Chinh thuc" : "On tap")
                        << " | Khoa: " << (de.locked ? "Co" : "Khong") << '\n';
                    found = true;
                }
            }
            if (!found)
            {
                SetColor(YELLOW);
                cout << "Khong tim thay de thi.\n";
                SetColor(WHITE);
            }
        }
        else if (option == 2)
        {
            cout << "1. Chinh thuc  2. On tap\n";
            int loai = nhapSo<int>("Chon loai: ");
            if (loai != 1 && loai != 2)
            {
                SetColor(RED);
                cout << "Lua chon khong hop le.\n";
                SetColor(WHITE);
                return;
            }
            bool found = false;
            for (const auto& de : thi.getMaDe())
            {
                if (de.loaiThi == loai)
                {
                    cout << "ID: " << de.id << " | Mon: " << de.tenMon
                        << " | Loai: " << (de.loaiThi == 1 ? "Chinh thuc" : "On tap") << '\n';
                    found = true;
                }
            }
            if (!found)
            {
                SetColor(YELLOW);
                cout << "Khong tim thay de thi loai nay.\n";
                SetColor(WHITE);
            }
        }
        else if (option == 3)
        {
            cout << "1. Khoa  2. Mo\n";
            int trangThai = nhapSo<int>("Chon trang thai: ");
            if (trangThai != 1 && trangThai != 2)
            {
                SetColor(RED);
                cout << "Lua chon khong hop le.\n";
                SetColor(WHITE);
                return;
            }
            bool found = false;
            for (const auto& de : thi.getMaDe())
            {
                // SỬA LẠI ĐIỀU KIỆN IF NHƯ SAU:
                if ((trangThai == 1 && de.locked) || (trangThai == 2 && !de.locked))
                {
                    cout << "ID: " << de.id << " | Mon: " << de.tenMon
                        << " | Trang thai: " << (de.locked ? "Khoa" : "Mo") << '\n';
                    found = true;
                }
            }
            if (!found)
            {
                SetColor(YELLOW);
                cout << "Khong tim thay de thi.\n";
                SetColor(WHITE);
            }
        }
    }
    void findExamRooms()
    {
        cout << "\n===== TIM PHONG THI =====\n";
        cout << "1. Tim theo suc chua (toi thieu)\n2. Tim theo trang thai\n3. Hien thi tat ca\n";
        int option = nhapSo<int>("Chon tieu chi: ");

        if (option == 1)
        {
            int minCapacity = nhapSo<int>("Nhap suc chua toi thieu: ");
            bool found = false;
            for (const auto& phong : examRooms)
            {
                if (phong.sucChua >= minCapacity)
                {
                    cout << "ID: " << phong.id << " | Ten: " << phong.tenPhong << " | Suc chua: " << phong.sucChua << " | Trang thai: " << (phong.locked ? "Khoa" : "Mo") << '\n';
                    found = true;
                }
            }
            if (!found)
                cout << "Khong tim thay phong.\n";
        }
        else if (option == 2)
        {
            cout << "1. Khoa  2. Mo\n";
            int trangThai = nhapSo<int>("Chon trang thai: ");
            if (trangThai != 1 && trangThai != 2)
            {
                cout << "Lua chon khong hop le.\n";
                return;
            }
            bool found = false;
            for (const auto& phong : examRooms)
            {
                if ((trangThai == 1 && phong.locked) || (trangThai == 2 && !phong.locked))
                {
                    cout << "ID: " << phong.id << " | Ten: " << phong.tenPhong << " | Suc chua: " << phong.sucChua << " | Trang thai: " << (phong.locked ? "Khoa" : "Mo") << '\n';
                    found = true;
                }
            }
            if (!found)
                cout << "Khong tim thay phong.\n";
        }
        else if (option == 3)
        {
            if (examRooms.empty())
            {
                cout << "Chua co phong thi nao.\n";
                return;
            }
            for (const auto& phong : examRooms)
            {
                cout << "ID: " << phong.id << " | Ten: " << phong.tenPhong << " | Suc chua: " << phong.sucChua << " | Trang thai: " << (phong.locked ? "Khoa" : "Mo") << '\n';
            }
        }
        else
            cout << "Lua chon khong hop le.\n";
    }

    // ==================== MENU ====================
    void adminMenu(QuanLyThi& thi)
    {
        int choice;
        do
        {
            SetColor(LIGHT_CYAN);
            cout << "\n╔═════════════════════════════════════════════╗\n";

            SetColor(LIGHT_YELLOW);
            cout << "║              MENU ADMIN TESTPRO             ║\n";

            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";

            SetColor(LIGHT_GREEN);
            cout << "║ [1]  Them giang vien                        ║\n";
            cout << "║ [2]  Xoa giang vien                         ║\n";
            cout << "║ [3]  Sua giang vien                         ║\n";
            cout << "║ [4]  Xem danh sach giang vien               ║\n";

            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";

            SetColor(LIGHT_BLUE);
            cout << "║ [5]  Them lop                               ║\n";
            cout << "║ [6]  Xoa lop                                ║\n";
            cout << "║ [7]  Sua lop                                ║\n";
            cout << "║ [8]  Xem danh sach lop                      ║\n";

            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";

            SetColor(LIGHT_MAGENTA);
            cout << "║ [9]  Xem thong ke he thong                  ║\n";
            cout << "║ [10] Thong ke sinh vien / giang vien        ║\n";
            cout << "║ [11] Quan ly phong thi                      ║\n";
            cout << "║ [12] Xuat bao cao thi                       ║\n";
            cout << "║ [13] Xem thong ke bao cao                   ║\n";

            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";

            SetColor(LIGHT_YELLOW);
            cout << "║ [14] Luu du lieu                            ║\n";
            cout << "║ [15] Tai du lieu                            ║\n";
            cout << "║ [16] Tim kiem sinh vien                     ║\n";
            cout << "║ [17] Loc de thi                             ║\n";
            cout << "║ [18] Tim phong thi                          ║\n";

            SetColor(LIGHT_RED);
            cout << "║ [0]  Thoat                                  ║\n";

            SetColor(LIGHT_CYAN);
            cout << "╚═════════════════════════════════════════════╝\n";

            SetColor(7);

            SetColor(WHITE);

            SetColor(WHITE);

            SetColor(WHITE);

            choice = nhapSo<int>("Chon chuc nang: ");

            switch (choice)
            {
            case 1:
                addTeacher();
                break;
            case 2:
                deleteTeacher();
                break;
            case 3:
                editTeacher();
                break;
            case 4:
                viewTeachers();
                break;
            case 5:
            {
                addClass();
                int l;
                do
                {
                    cout << "\n================ Quan ly sinh vien ================\n";
                    cout << "1. Them sinh vien\n";
                    cout << "2. Xoa sinh vien\n";
                    cout << "3. Sua sinh vien\n";
                    cout << "4. Xem danh sach sinh vien\n";
                    cout << "0. Thoat\n";
                    l = nhapSo<int>("Chon chuc nang: ");
                    switch (l)
                    {
                    case 1:
                        addStudent();
                        break;
                    case 2:
                        deleteStudent();
                        break;
                    case 3:
                        editStudent();
                        break;
                    case 4:
                        viewStudents();
                        break;
                    case 0:
                        break;
                    default:
                        cout << "Lua chon khong hop le.\n";
                    }
                } while (l != 0);
            }
            break;
            case 6:
                deleteClass();
                break;
            case 7:
                editClass();
                break;
            case 8:
                viewClasses();
                break;
            case 9:
                viewSystemStatistics();
                break;
            case 10:
                countUsers();
                break;
            case 11:
                manageExamRooms();
                break;
            case 12:
                exportExamReport(thi);
                break;
            case 13:
                viewExamStatistics(thi);
                break;
            case 14:
                saveData();
                thi.saveDeThi();
                break;
            case 15:
                loadData();
                break;
            case 16:
                searchStudent();
                break;
            case 17:
                filterExams(thi);
                break;
            case 18:
                findExamRooms();
                break;
            case 0:
                cout << "Thoat chuong trinh.\n";
                break;
            default:
                cout << "Lua chon khong hop le.\n";
            }
        } while (choice != 0);
    }
};
void drawLine(int color)
{
    SetColor(color);
    cout << "====================================================\n";
    SetColor(WHITE);
}

//==================== MAIN ====================
int main()
{
    SetConsoleOutputCP(CP_UTF8);

    AdminSystem adminSystem;
    QuanLyNguoiDung ql;
    QuanLyThi thi;

    adminSystem.loadStudents("sinhvien.txt");
    adminSystem.loadTeachers("giangvien.txt");
    adminSystem.loadClasses("lophoc.txt");
    adminSystem.loadRooms("phongthi.txt");

    adminSystem.loadRanking();
    adminSystem.loadKetQua();

    thi.loadDeThi();

    int vaiTro;

    SetColor(LIGHT_CYAN);

    cout << R"(

  /\_/\     ████████╗███████╗███████╗████████╗██████╗ ██████╗  ██████╗ 
 ( >.< )    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝██╔══██╗██╔══██╗██╔═══██╗
  > ^ <        ██║   █████╗  ███████╗   ██║   ██████╔╝██████╔╝██║   ██║
 /     \       ██║   ██╔══╝  ╚════██║   ██║   ██╔═══╝ ██╔══██╗██║   ██║
               ██║   ███████╗███████║   ██║   ██║     ██║  ██║╚██████╔╝ 
               ╚═╝   ╚══════╝╚══════╝   ╚═╝   ╚═╝     ╚═╝  ╚═╝ ╚═════╝ 

)" << endl;

    do
    {

        drawLine(11);

        SetColor(LIGHT_GREEN);
        cout << "        HE THONG QUAN LY THI TRAC NGHIEM\n";

        drawLine(11);

        SetColor(LIGHT_BLUE);
        cout << "        [1] DANG NHAP ADMIN\n";

        SetColor(LIGHT_YELLOW);
        cout << "        [2] DANG NHAP GIANG VIEN\n";

        SetColor(LIGHT_MAGENTA);
        cout << "        [3] DANG NHAP SINH VIEN\n";

        SetColor(LIGHT_RED);
        cout << "        [0] THOAT CHUONG TRINH\n";

        SetColor(WHITE);

        vaiTro = nhapSo<int>("\nNhap lua chon: ");

        if (vaiTro == 1)
        {

            if (adminSystem.loginAdmin())
            {
                adminSystem.adminMenu(thi);
            }
        }
        else if (vaiTro == 2)
        {

            ql.layDanhSach().clear();

            for (auto& t : adminSystem.getTeachers())
            {
                ql.them(&t);
            }

            Person* user = ql.dangNhap();

            if (user != nullptr)
            {

                SetColor(GREEN);
                cout << "Dang nhap thanh cong!\n";
                SetColor(WHITE);

                GiangVien* gv = dynamic_cast<GiangVien*>(user);

                if (gv != nullptr)
                {
                    gv->menu(adminSystem.getStudents(), adminSystem.getClasses(), adminSystem.getRooms(), thi);
                }
            }
            else
            {

                SetColor(RED);
                cout << "Dang nhap that bai!\n";
                SetColor(WHITE);
            }
        }
        else if (vaiTro == 3)
        {

            ql.layDanhSach().clear();

            for (auto& hs : adminSystem.getStudents())
            {
                ql.them(&hs);
            }

            Person* user = ql.dangNhap();

            if (user != nullptr)
            {

                SetColor(GREEN);
                cout << "Dang nhap thanh cong!\n";
                SetColor(WHITE);

                SinhVien* hs = dynamic_cast<SinhVien*>(user);

                if (hs != nullptr)
                {
                    hs->menu(thi);
                }
            }
            else
            {

                SetColor(RED);
                cout << "Dang nhap that bai!\n";
                SetColor(WHITE);
            }
        }

    } while (vaiTro != 0);

    return 0;
}