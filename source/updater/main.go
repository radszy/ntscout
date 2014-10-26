package main

import (
	"archive/zip"
	"bytes"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

const APP_VERSION = "0.1"

var appver *bool = flag.Bool("v", false, "Print the version number.")
var ntsver *float64 = flag.Float64("n", 0.0, "Version of NTScout")
var user *string = flag.String("u", "", "Username used to login.")
var pass *string = flag.String("p", "", "Password used to login.")

var pwd, _ = filepath.Abs(filepath.Dir(os.Args[0]))
var tmp = os.TempDir() + "\\NTScout"

var buf bytes.Buffer
var logger = log.New(&buf, "logger: ", log.Lshortfile)

func main() {
	flag.Parse()

	if *appver {
		fmt.Println("Version:", APP_VERSION)
		return
	}

	if *user == "" || *pass == "" || *ntsver == 0.0 {
		fmt.Println("You need to specify username, password and version of NTScout.")
		flag.PrintDefaults()
		return
	}

	logger.Println("Updater version:", APP_VERSION)
	logger.Println("NTScout version:", *ntsver)

	fmt.Print("Updating binary files... ")
	if updateBinary() {
		fmt.Println("ok.")
	} else {
		fmt.Println()
	}

	fmt.Print("Updating country data... ")
	if updateCountry() {
		fmt.Println("ok.")
	} else {
		fmt.Println()
	}

	cmd := exec.Command(pwd+"/NTScout.exe", "--post-update")
	err := cmd.Start()
	if err != nil {
		logger.Println(err)
	}

	os.Mkdir("logs", 0777)

	timestr := strconv.FormatInt(time.Now().Unix(), 10)
	file, err := os.Create("logs/updater-" + timestr)
	if err != nil {
		fmt.Println("Could not create logfile!")
	}
	defer file.Close()
	file.Write(buf.Bytes())
}

func updateCountry() bool {
	if !Login(user, pass) {
		return false
	}

	countries := Countries()
	if countries == nil {
		return false
	}

	names := TranslatedNames()
	if names == nil {
		return false
	}

	length := len(countries.Country)
	for i := 0; i < length; i++ {
		countries.Country[i].NameEn = names[i]
	}

	file, err := os.OpenFile(pwd+"/data/country.dat", os.O_WRONLY|os.O_TRUNC, 0777)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer file.Close()

	for _, c := range countries.Country {
		file.WriteString(c.ID + "," + c.Name + "," + c.NameEn + "," +
			c.Divisions + "," + c.Users + "\n")

		_, err := os.Stat("flags/flag_" + c.ID + ".gif")
		if os.IsNotExist(err) {
			DownloadFlags(c.ID, pwd)
		}
	}

	return true
}

func updateBinary() bool {
	url := Releases(*ntsver)
	if url == "" {
		return false
	}

	if !DownloadRelease(url, os.TempDir()) ||
		!unzip(tmp+".zip", os.TempDir()) ||
		!removeOldFiles() || !moveNewFiles() {
		return false
	}
	clean()

	return true
}

func removeOldFiles() bool {
	entries, err := ioutil.ReadDir(pwd)
	if err != nil {
		logger.Println(err)
		return false
	}

	for _, entry := range entries {
		// don't delete updater while it's running (error on Windows)
		// results are just for the user so keep them
		if entry.Name() == "Updater.exe" || entry.Name() == "results" {
			continue
		}
		err = os.RemoveAll(pwd + "/" + entry.Name())
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	return true
}

func moveNewFiles() bool {
	entries, err := ioutil.ReadDir(tmp)
	if err != nil {
		logger.Println(err)
		return false
	}

	for _, entry := range entries {
		if entry.Name() == "Updater.exe" {
			err = os.Rename(tmp+"/"+entry.Name(), pwd+"/"+entry.Name()+"-new")
			if err != nil {
				logger.Println(err)
				return false
			}
			continue
		}
		err = os.Rename(tmp+"/"+entry.Name(), pwd+"/"+entry.Name())
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	return true
}

func clean() {
	err := os.RemoveAll(tmp)
	if err != nil {
		logger.Println(err)
	}

	err = os.Remove(tmp + ".zip")
	if err != nil {
		logger.Println(err)
	}
}

func unzip(src, dest string) bool {
	r, err := zip.OpenReader(src)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer r.Close()

	for _, f := range r.File {
		rc, err := f.Open()
		if err != nil {
			logger.Println(err)
			return false
		}
		defer rc.Close()

		fpath := filepath.Join(dest, f.Name)
		if f.FileInfo().IsDir() {
			os.MkdirAll(fpath, f.Mode())
		} else {
			var fdir string
			if lastIndex := strings.LastIndex(fpath, string(os.PathSeparator)); lastIndex > -1 {
				fdir = fpath[:lastIndex]
			}

			err = os.MkdirAll(fdir, f.Mode())
			if err != nil {
				logger.Println(err)
				return false
			}
			f, err := os.OpenFile(
				fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
			if err != nil {
				logger.Println(err)
				return false
			}
			defer f.Close()

			_, err = io.Copy(f, rc)
			if err != nil {
				logger.Println(err)
				return false
			}
		}
	}
	return true
}
